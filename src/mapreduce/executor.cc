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
#include <list>
#include <unordered_map>

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
    auto network_size = GET_VEC_STR("network.nodes").size();
    path_lib += ("/" + m->library);
    DL_loader loader (path_lib);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      context.logger->error ("Not found library path[%s]", path_lib.c_str());
    }

    before_map_t _before_map_ = loader.load_function_before_map("before_map");
    std::unordered_map<std::string, void*> options;
    if(_before_map_ != nullptr)
      _before_map_(options);

    mapper_t _map_ = loader.load_function(m->func_name);
    stringstream ss (input);

    char next_line[10000]; //! :TODO: change to DFS line limit
    velox::MapOutputCollection results;

    while (!ss.eof()) {
      bzero(next_line, 10000);
      ss.getline (next_line, 10000);
      if (strnlen(next_line, 10000) == 0) 
        continue;

      std::string line(next_line);
      _map_ (line, results, options);
    }

    vector<uint32_t> keys_per_node;
    vector<string> headers_list;
    keys_per_node.resize(network_size);
    headers_list.resize(network_size);

    auto run_headers = [&headers_list, &keys_per_node, &network_size](std::string key, std::vector<std::string>* value) mutable {
      int node = h(key) % network_size;
      keys_per_node[node]++;
      headers_list[node] = key;
    };
    results.travel(run_headers);

    int i = 0;
    for(unsigned int node = 0; node < network_size; node++) {
      if(keys_per_node[node] == 0) continue;

      KeyValueShuffle kv; 
      kv.job_id_ = m->job_id;           // :TODO:
      kv.map_id_ = 0;
      kv.key_ = headers_list[node];
      kv.is_header = true;
      kv.number_of_keys = keys_per_node[node];
      peer->process(&kv);
      i++;
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

    after_map_t _after_map_ = loader.load_function_after_map("after_map");
    if(_after_map_ != nullptr)
      _after_map_(options);

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

  reducer_t _reducer_ = loader.load_function_reduce(task->func_name);

  try {
    IReader ireader;
    ireader.set_job_id(task->job_id);
    ireader.set_map_id(0);                 // :TODO:
    ireader.set_reducer_id(0);
    ireader.init();

    uint32_t iterations = 0;
    uint32_t total_size = 0;
    std::string block_content;

    std::list<std::string> values;
    while (ireader.is_next_key()) {
      string key;
      ireader.get_next_key(key);

      //int total_iterations = 0;
      velox::MapOutputCollection output;
      values.clear();

      //if (ireader.is_next_value()) {
        //ireader.get_next_value(last_output);
        //total_iterations = 1;

        //TODO: make a function to get values at a time
        while (ireader.is_next_value()) {
          string value;
          ireader.get_next_value(value);
          values.push_back(value);

          //total_iterations++;
        }

      if(values.size() > 0)
        _reducer_ (key, values, output);
      //}

      std::string current_block_content;
      auto make_block_content = [&current_block_content](std::string key, std::vector<std::string>* values) mutable {
        for(std::string& value : *values) 
          current_block_content += key + ": " + value + "\n";
      };

      output.travel(make_block_content);

      //INFO("Key %s #iterations: %i", key.c_str(), total_iterations);
      if (block_content.length() + current_block_content.length() > (uint32_t)block_size || current_block_content.length() > (uint32_t)block_size) {
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
        block_content += current_block_content;
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

      block_content += current_block_content;
    }

    FileInfo fi;
    fi.name = task->file_output;
    fi.num_block = iterations;
    fi.size = total_size;
    fi.hash_key = h(fi.name);
    fi.replica = 1;
    fi.reducer_output = true;
    fi.job_id = task->job_id;

    dynamic_cast<PeerMR*>(peer)->process(&fi);

  } catch (std::exception& e) {
    context.logger->error ("Error in the executer: %s", e.what());
    exit(EXIT_FAILURE);
  }
  return true;
}
// }}}
} /* eclipse  */
