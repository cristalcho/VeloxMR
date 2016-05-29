#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/hash.hh"
#include "../mapreduce/messages/key_value_shuffle.h"
#include "../mapreduce/fs/ireader.h"
#include "../messages/keyvalue.hh"

#include <string>
#include <sstream>
#include <utility>
#include <functional>

using namespace eclipse;
using namespace std;

uint32_t eclipse::Executor::map_ids(0);

namespace eclipse {
// Constructor {{{
Executor::Executor(PeerMR* p) : peer(p) { }
Executor::~Executor() { }
// }}}
// run_map {{{
bool Executor::run_map (messages::Task* m, std::string input) {
    auto path_lib = context.settings.get<string>("path.applications");
    path_lib += ("/" + m->library);
    DL_loader loader (path_lib);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      context.logger->error ("Not found library path[%s]", path_lib.c_str());
    }

    function<pair<string, string>(string)> _map_ = 
        loader.load_function(m->func_name);
    stringstream ss (input);

    while (!ss.eof()) {
      char* next_line =  new char[256]; //! :TODO: change to DFS line limit
      bzero(next_line, 256);
      ss.getline (next_line, 256);
      if (strnlen(next_line, 256) == 0) 
        continue;

      pair<string, string> key_value = _map_ (string(next_line));

      auto key        = key_value.first;
      auto hash_key   = h(key.c_str());
      auto& value     = key_value.second;
      context.logger->info ("Generated value: %s -> %s", next_line, value.c_str());

      KeyValueShuffle kv; 
      kv.job_id_ = m->job_id;
      kv.map_id_ = 0;
      kv.key_ = key;
      kv.value_ = value; 
      peer->process(&kv);
      delete next_line;
    }

    return true;
  }
// }}}
// run_reduce {{{
bool Executor::run_reduce (messages::Task* task) {
    auto path_lib = context.settings.get<string>("path.applications");
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
    ireader.set_map_id(task->map_id);
    ireader.set_reducer_id(0);
    ireader.init();

    while (ireader.is_next_key()) {
      string key;
      ireader.get_next_key(key);

      int total_iterations = 0;
      string last_output;
      if (ireader.is_next_value()) 
          ireader.get_next_value(last_output);

      while (ireader.is_next_value()) {
        string value;
        ireader.get_next_value(value);

        last_output = _reducer_ (value, last_output);

        total_iterations++;
      }
      context.logger->info ("Key %s #iterations: %i", key.c_str(), total_iterations);

      FileInfo fi;
      fi.file_name = key;
      fi.num_block = 1;
      fi.file_size = last_output.length();
      fi.file_hash_key = h(key.c_str());

      BlockInfo bi;
      bi.file_name = key;
      bi.block_name = key + "_0";
      bi.block_seq = 0;
      bi.block_hash_key = h(bi.block_name);
      bi.block_size = last_output.length();
      bi.content = last_output;

      dynamic_cast<PeerDFS*>(peer)->process(&fi);
      dynamic_cast<PeerDFS*>(peer)->insert_block(&bi);
    }
    } catch (std::exception& e) {
      context.logger->error ("Error in the executer: %s", e.what());
      exit(1);
    }
}
// }}}
} /* eclipse  */
