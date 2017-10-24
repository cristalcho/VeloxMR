#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/histogram.hh"
#include "../common/hash.hh"
#include "../mapreduce/output_collection.hh"
#include "../mapreduce/fs/ireader.h"
#include "messages/key_value_shuffle.h"
#include "../client/dfs.hh"
#include "task_cxx.hh"
#include "task_python.hh"

#include <exception>
#include <string>
#include <sstream>
#include <utility>
#include <functional>
#include <queue>
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
  std::unordered_map<std::string, void*> options;
  vector<uint32_t> keys_blocks;
  queue<std::thread> threads;
  mutex mut;

  auto network_size = GET_VEC_STR("network.nodes").size();
  size_t mappers = GET_INT("mapreduce.mappers");

  task_handler* task_execution = nullptr;

  if (m->lang == "C++") {
    task_execution = new task_cxx(m->library, m->func_name);
  } else {
    task_execution = new task_python(m->func_body, m->before_map, m->after_map);
  }
  task_execution->setup(true);

  INFO("Launching mapper with %i threads", m->blocks.size());

  task_execution->before_map(options);

  try {
  for (size_t map_id = 0; map_id < m->blocks.size(); map_id++) {

    // Make sure we only execute 'mappers' threads at that time
    if (threads.size() >= mappers) {
      threads.front().join();
      threads.pop();
    }

    threads.emplace([&, this] (size_t id) {
          try {

          {
          auto* kv_blocks = new map<uint32_t, KeyValueShuffle>();

          {
            const string block_name = m->blocks[id].second;
            Local_io local_io;
            string input = local_io.read(block_name);
            istringstream ss (std::move(input));

            velox::OutputCollection results;
            char* next_line = new char[MAP_MAX_LINE];

            while (!ss.eof()) {
              bzero(next_line, MAP_MAX_LINE);
              ss.getline (next_line, MAP_MAX_LINE);
              if (strnlen(next_line, MAP_MAX_LINE) == 0) 
                continue;

              std::string line(next_line);
              task_execution->map(line, results, options);
            }
            delete[] next_line;

            for (auto& pair : results) {
              auto& key = pair.first;
              auto& value = pair.second;
              uint32_t node = h(key) % network_size;
              auto it = kv_blocks->find(node);
              if (it == kv_blocks->end()) {
                it = kv_blocks->insert({node, {}}).first;

                it->second.node_id = node;
                it->second.job_id_ = m->job_id;
                it->second.map_id_ = 0;
                it->second.origin_id = context.id;
              }

              it->second.kv_pairs.insert({key, std::move(value)});
            }
          }

          mut.lock();
          for (auto it: *kv_blocks)
              keys_blocks.push_back(it.first);
          mut.unlock();

          vector<int> shuffled_array;

          {
            for (uint32_t i = 0; i < network_size; i++)
              shuffled_array.push_back(i);

            mut.lock();
            unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
            auto engine = std::default_random_engine{seed1};
            std::shuffle(shuffled_array.begin(), shuffled_array.end(), engine);
            mut.unlock();
          }

          for (auto& index : shuffled_array) {
            auto it = kv_blocks->find(index);
            if (it != kv_blocks->end()) {
              KeyValueShuffle* kv = &(it->second);
              peer->insert_key_value(kv);
            }
          }

          //auto it = kv_blocks->begin();
          //while (it != kv_blocks->end()) {
          //  peer->insert_key_value(it->second.get());
          //  ++it;
          //}

//          kv_blocks->clear();
          delete kv_blocks;
          }
          INFO("Finishing map threads");

        } catch (exception& e) {
          ERROR("Mapper exception %s", e.what());
        } catch (boost::exception& e) {
          ERROR("Mapper exception %s", diagnostic_information(e).c_str());
        }

    }, map_id);
  }

    while (!threads.empty()) {
      threads.front().join();
      threads.pop();
    }

    task_execution->after_map(options);
    delete task_execution;

    peer->notify_map_is_finished(m->job_id, keys_blocks);
    keys_blocks.clear();

  } catch (exception& e) {
    ERROR("Mapper parent exception %s", e.what());
  } catch (boost::exception& e) {
    ERROR("Mapper parent exception %s", diagnostic_information(e).c_str());
  }

  return true;
}
// }}}
// run_reduce {{{
bool Executor::run_reduce (messages::Task* task) {
  auto block_size = GET_INT("filesystem.block");
  //auto reducer_slot = GET_INT("mapreduce.reduce_slot");

  auto network_size = GET_VEC_STR("network.nodes").size();
  Histogram boundaries(network_size, 100);
  boundaries.initialize();
  velox::model::metadata metadata;

  task_handler* task_execution = nullptr;

  if (task->lang == "C++") {
    task_execution = new task_cxx(task->library, task->func_name);
  } else {
    task_execution = new task_python(task->func_body, task->before_map, task->after_map);
  }
  task_execution->setup(false);

  uint32_t total_size = 0;
  uint32_t num_keys = 0;
  queue<std::thread> threads;
  DirectoryMR directory;
  uint32_t reducer_slot = directory.select_number_of_reducers(task->job_id);
  mutex mut;
  DEBUG("LAunching reducer with %i threads", reducer_slot);
  for (uint32_t reducer_id = 0; reducer_id < reducer_slot; reducer_id++) {

    if (threads.size() >= 1) {
      threads.front().join();
      threads.pop();
    }
    threads.emplace(std::thread([&, this] (int id) {

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
            task_execution->reduce(key, values, output);

          } catch (std::exception& e) {
            ERROR("Error in the executer: %s", e.what());
            exit(EXIT_FAILURE);
          }
        } else 
          INFO("REDUCER skipping a KEY");

        std::string current_block_content = "";

        auto make_block_content = [&] (std::string key, std::vector<std::string> values) mutable {

          for(std::string& value : values)  {
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

  while (!threads.empty()) {
    threads.front().join();
    threads.pop();
  }

  velox::DFS dfs;
  INFO("REDUCER APPENDING FILE_METADATA KP:%u", num_keys);
  dfs.file_metadata_append(task->file_output, total_size, metadata);

  peer->notify_task_leader(task->leader, task->job_id, "REDUCE");


  return true;
}
// }}}
} /* eclipse  */
