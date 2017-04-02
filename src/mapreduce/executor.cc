#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/histogram.hh"
#include "../common/hash.hh"
#include "../mapreduce/output_collection.hh"
#include "../mapreduce/fs/ireader.h"
#include "../messages/keyvalue.hh"
#include "../client/dfs.hh"

#include <exception>
#include <string>
#include <sstream>
#include <utility>
#include <functional>
#include <list>
#include <unordered_map>
#include <mutex>
#include <boost/exception/exception.hpp>
#define MAP_MAX_LINE 10000

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(TaskExecutor* p) : peer(p) { }
Executor::~Executor() { }
// }}}
// run_map {{{
bool Executor::run_map (messages::Task* m) {

  auto path_lib = GET_STR("path.applications");
  auto network_size = GET_VEC_STR("network.nodes").size();
  auto mappers = GET_INT("mapreduce.mappers");
  path_lib += ("/" + m->library);
  DL_loader loader (path_lib);

  try {
    loader.init_lib();
  } catch (std::exception& e) {
    context.logger->error ("Not found library path[%s]", path_lib.c_str());
  }

  before_map_t _before_map_ = loader.load_function_before_map("before_map");

  vector<uint32_t> keys_blocks;

  vector<std::thread> threads;
  mutex mut;
  INFO("LAunching mapper with %i threads", mappers);
  for (int reducer_id = 0; reducer_id < m->blocks.size(); reducer_id++) {

    // Make sure we only execute 'mappers' threads at that time
    if (threads.size() >= mappers) {
      threads.front().join();
      threads.erase(threads.begin(), threads.begin()+1);
    }

    threads.emplace_back(std::thread([&] (int id) {
          std::mt19937 rng;
          rng.seed(std::random_device()());
          std::uniform_int_distribution<std::mt19937::result_type> dist(0, INT_MAX);

          std::unordered_map<std::string, void*> options;
          if(_before_map_ != nullptr)
            _before_map_(options);

          mapper_t _map_ = loader.load_function(m->func_name);

          const string block_name = m->blocks[id].second;
          INFO("Executing map on block: %s", block_name.c_str());
          Local_io local_io;
          string input = local_io.read(block_name);
          stringstream ss (input);

          char next_line[MAP_MAX_LINE];
          velox::OutputCollection results;

          while (!ss.eof()) {
            bzero(next_line, MAP_MAX_LINE);
            ss.getline (next_line, MAP_MAX_LINE);
            if (strnlen(next_line, MAP_MAX_LINE) == 0) 
              continue;

            std::string line(next_line);
            _map_ (line, results, options);
          }

          map<uint32_t, KeyValueShuffle> kv_blocks;
          try {

          auto run_block = [&mut, &m, &kv_blocks, &keys_blocks, network_size, &dist, &rng](std::string key, std::vector<std::string>* value) mutable {
            int node = h(key) % network_size;
            auto it = kv_blocks.find(node);
            if (it == kv_blocks.end()) {
              it = kv_blocks.insert({node, {}}).first;

              it->second.node_id = node;
              it->second.job_id_ = m->job_id;
              it->second.map_id_ = 0;

              uint32_t random_id = dist(rng);
              mut.lock();
              keys_blocks.push_back(node);
              mut.unlock();
              it->second.kv_id = random_id;
            }

            it->second.kv_pairs.insert({key, std::move(*value)});
          };
          results.travel(run_block);

          vector<int> shuffled_array;

          for (int i = 0; i < network_size; i++)
            shuffled_array.push_back(i);

          auto engine = std::default_random_engine{};
          std::shuffle(shuffled_array.begin(), shuffled_array.end(), engine);
                

          for (auto& index: shuffled_array) {
            mut.lock();
            auto it = kv_blocks.find(index);
            if (it != kv_blocks.end()) {
              peer->insert_key_value(&(it->second));
            }
            mut.unlock();
          }


          after_map_t _after_map_ = loader.load_function_after_map("after_map");
          if(_after_map_ != nullptr)
            _after_map_(options);
          INFO("MAP thread finishing");

          } catch (exception& e) {
            INFO("Mapper exception %s", e.what());
          } catch (boost::exception& e) {
            INFO("Mapper exception %s", diagnostic_information(e).c_str());
          }


    }, reducer_id));
  }

  try {
    for (auto& thread : threads)
      thread.join();

    peer->notify_map_is_finished(m->job_id, keys_blocks);
  } catch (exception& e) {
    INFO("Mapper parent exception %s", e.what());
          } catch (boost::exception& e) {
            INFO("Mapper parent exception %s", diagnostic_information(e).c_str());
          }
  return true;
}
// }}}
// run_reduce {{{
bool Executor::run_reduce (messages::Task* task) {
  auto path_lib = context.settings.get<string>("path.applications");
  auto block_size = GET_INT("filesystem.block");
  //auto reducer_slot = GET_INT("mapreduce.reduce_slot");
  path_lib += ("/" + task->library);
  DL_loader loader (path_lib);

  auto network_size = GET_VEC_STR("network.nodes").size();
  Histogram boundaries(network_size, 100);
  boundaries.initialize();
  velox::model::metadata metadata;

  try {
    loader.init_lib();
  } catch (std::exception& e) {
    context.logger->error ("Not found library path[%s]", path_lib.c_str());
  }

  reducer_t _reducer_ = loader.load_function_reduce(task->func_name);

  uint32_t total_size = 0;
  uint32_t num_keys = 0;
  vector<std::thread> threads;
  DirectoryMR directory;
  uint32_t reducer_slot = directory.select_number_of_reducers(task->job_id);
  mutex mut;
  DEBUG("LAunching reducer with %i threads", reducer_slot);
  for (int reducer_id = 0; reducer_id < reducer_slot; reducer_id++) {
    threads.push_back(std::thread([&] (int id) {
          DEBUG("%i %i", task->job_id, id);                                                                                                                                                                       

      IReader ireader;
      ireader.set_job_id(task->job_id);
      ireader.set_map_id(0);                 // :TODO:
      ireader.set_reducer_id(id);

      mut.lock();
      ireader.init();
      mut.unlock();
      std::string block_content;

      std::vector<std::string> values;
      while (ireader.is_next_key()) {
        string key;
        ireader.get_next_key(key);

        mut.lock();
        DEBUG("PROCCESSING KEY %s", key.c_str());
        mut.unlock();

        velox::OutputCollection output;
        values.clear();

        //TODO: make a function to get values at a time
        while (ireader.is_next_value()) {
          string value;
          ireader.get_next_value(value);
          values.push_back(value);
        }
        DEBUG("RUNNING REDUCER %s", key.c_str());

        if(values.size() > 0) {
          try {
            _reducer_ (key, values, output);

          } catch (std::exception& e) {
            ERROR("Error in the executer: %s", e.what());
            exit(EXIT_FAILURE);
          }
        } else 
          INFO("REDUCER skipping a KEY");

        std::string current_block_content = "";

        auto make_block_content = [&] (std::string key, std::vector<std::string>* values) mutable {

          for(std::string& value : *values)  {
            current_block_content += key + ": " + value + "\n";
            num_keys++;
          }
        };

        output.travel(make_block_content);

        block_content += current_block_content;
        mut.lock();
        total_size += current_block_content.length();
        mut.unlock();


        if (block_content.length() > (uint32_t)block_size || !ireader.is_next_key()) {
          string name = task->file_output + "-" + key;

          mut.lock();
          metadata.blocks.push_back(name);
          metadata.hash_keys.push_back(boundaries.random_within_boundaries(peer->get_id()));
          metadata.block_size.push_back(block_content.length());
          mut.unlock();

          INFO("REDUCER SAVING TO DISK %s : %lu B", name.c_str(), block_content.size());
          Local_io local_io;
          local_io.write(name, block_content);

          block_content.clear();
        }

      }
    }, reducer_id));
  }

  for (auto& thread : threads)
    thread.join();

  velox::DFS dfs;
  INFO("REDUCER APPENDING FILE_METADATA KP:%u", num_keys);
  dfs.file_metadata_append(task->file_output, total_size, metadata);

  peer->notify_task_leader(task->leader, task->job_id, "REDUCE");


  return true;
}
// }}}
} /* eclipse  */
