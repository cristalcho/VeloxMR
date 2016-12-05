#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/hash.hh"
#include "../mapreduce/messages/key_value_shuffle.h"
#include "../mapreduce/map_output_collection.hh"
#include "../mapreduce/fs/ireader.h"
#include "../messages/keyvalue.hh"

#include <string>
#include <sstream>
#include <utility>
#include <functional>

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(PeerMR* p) : peer(p) { }
Executor::~Executor() { }
// }}}
// run_map {{{
bool Executor::run_map (messages::Task* m, std::string input) {
    auto path_lib = GET_STR("path.applications");
    path_lib += ("/" + m->library);
    DL_loader loader (path_lib);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      context.logger->error ("Not found library path[%s]", path_lib.c_str());
    }

    mapper_t _map_ = loader.load_function(m->func_name);
    stringstream ss (input);

    char next_line[10000]; //! :TODO: change to DFS line limit
    velox::MapOutputCollection results;

    while (!ss.eof()) {
      bzero(next_line, 10000);
      ss.getline (next_line, 10000);
      if (strnlen(next_line, 10000) == 0) 
        continue;

      _map_ (string(next_line), results);
    }

    auto run_block = [&m, &peer = this->peer](std::string key, std::vector<std::string>* value) mutable {
      KeyValueShuffle kv; 
      kv.job_id_ = m->job_id;           // :TODO:
      kv.map_id_ = 0;
      kv.key_ = key;
      kv.value_ = std::move(*value);
      peer->process(&kv);
    };

    results.travel(run_block);

    return true;
  }
// }}}
// run_reduce {{{
bool Executor::run_reduce (messages::Task* task) {
  auto path_lib = context.settings.get<string>("path.applications");
  auto block_size = GET_INT("filesystem.block");
  path_lib += ("/" + task->library);
  DL_loader loader (path_lib);

  try {
    loader.init_lib();
  } catch (std::exception& e) {
    context.logger->error ("Not found library path[%s]", path_lib.c_str());
  }

  function<string(string,string)> _reducer_ = 
    loader.load_function_reduce(task->func_name);

  try {

    IReader ireader;
    ireader.set_job_id(task->job_id);
    ireader.set_map_id(0);                 // :TODO:
    ireader.set_reducer_id(0);
    ireader.init();

    uint32_t iterations = 0;
    uint32_t total_size = 0;
    std::string block_content;
    while (ireader.is_next_key()) {
      string key;
      ireader.get_next_key(key);

      int total_iterations = 0;
      string last_output;
      if (ireader.is_next_value()) {
        ireader.get_next_value(last_output);
        total_iterations = 1;

        while (ireader.is_next_value()) {
          string value;
          ireader.get_next_value(value);

          last_output = _reducer_ (value, last_output);

          total_iterations++;
        }
      }
      INFO("Key %s #iterations: %i", key.c_str(), total_iterations);

      if (block_content.length() + last_output.length() > (uint32_t)block_size) {
        BlockInfo bi;
        bi.file_name = task->file_output;
        bi.name = task->file_output + "-" + key.c_str();
        bi.seq = iterations;
        bi.hash_key = h(bi.name);
        bi.size = block_content.length();
        bi.content = block_content;
        bi.replica = 1;
        bi.node = "";
        bi.l_node = "";
        bi.r_node = "";
        bi.is_committed = 1;

        dynamic_cast<PeerMR*>(peer)->submit_block(&bi);
        iterations++;
        total_size += block_content.length();
        block_content = "";

      } else if (!ireader.is_next_key()) {
        block_content += key + ":" + last_output + "\n";
        BlockInfo bi;
        bi.file_name = task->file_output;
        bi.name = task->file_output + "-" + key.c_str();
        bi.seq = iterations;
        bi.hash_key = h(bi.name);
        bi.size = block_content.length();
        bi.content = block_content;
        bi.replica = 1;
        bi.node = "";
        bi.l_node = "";
        bi.r_node = "";
        bi.is_committed = 1;

        dynamic_cast<PeerMR*>(peer)->submit_block(&bi);
        iterations++;
        total_size += block_content.length();
        block_content = "";
      }

      block_content += key + ":" + last_output + "\n";

    }

    FileInfo fi;
    fi.name = task->file_output;
    fi.num_block = iterations;
    fi.size = total_size;
    fi.hash_key = h(fi.name);
    fi.replica = 1;

    dynamic_cast<PeerMR*>(peer)->process(&fi);

  } catch (std::exception& e) {
    context.logger->error ("Error in the executer: %s", e.what());
    exit(EXIT_FAILURE);
  }
  return true;
}
// }}}
} /* eclipse  */
