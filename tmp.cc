#include "executor.hh"
#include "dl_loader.hh"
#include "common/histogram.hh"
#include "common/hash.hh"
#include "../mapreduce/output_collection.hh"
#include "../mapreduce/fs/ireader.h"
#include "messages/key_value_shuffle.h"
#include "client/dfs.hh"
#include "task_cxx.hh"
#include <libmemcached/memcached.h>

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
#include <random>

#define ITERATIONS 5
#define NUM_SERVERS 9 //7
#define NUM_CLUSTERS 9
#define MAP_MAX_LINE 10000

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(TaskExecutor* p, std::unordered_multimap<std::string, void*>** uma, std::vector<std::string>* mp) : peer(p), umaparray(uma), memoryPoints(mp){}
Executor::~Executor() { }
// }}}

// With_Memcached {{{
static string group_keys[NUM_SERVERS] = {"dicl", "peach", "no", "dfs", "map", "hadoop", "pear", "memc", "apple"};
inline memcached_st *_get_memcached() { 
    const char *server = "--SERVER=dumbo051 --SERVER=dumbo052 --SERVER=dumbo053 --SERVER=dumbo054 --SERVER=dumbo055 --SERVER=dumbo056 --SERVER=dumbo057 --SERVER=dumbo058 --SERVER=dumbo059";
    return memcached(server, strlen(server));
}
// }}}

// run_map {{{
bool Executor::run_map (messages::Task* m) {
  unordered_map<std::string, void*> options;
  vector<uint32_t> keys_blocks;
  queue<thread> threads;
  mutex mut;

  auto network_size = GET_VEC_STR("network.nodes").size();
  size_t mappers = GET_INT("mapreduce.mappers");

  const int len = 256;
  char* hostname = new char[len];
  gethostname(hostname, len);
  int mapHost = hostname[7]-'0';
  mapHost -= 1;

  task_handler* task_execution = nullptr;

  if (m->lang == "C++") {
    task_execution = new task_cxx(m->library, m->func_name);
  } else {
//    task_execution = new task_python(m->func_body, m->before_map, m->after_map);
  }
  task_execution->setup(true);

  INFO("Launching mapper with %i threads", m->blocks.size());

  task_execution->before_map(options);
  velox::model::metadata md = velox::get_metadata(m->input_path, VELOX_LOGICAL_OUTPUT);
  const int job_id = m->job_id;

//soojeong
  size_t value_length;
  uint32_t flags;
  memcached_return_t rt;
  memcached_st *memc = _get_memcached();
  if(memc==NULL){ INFO("FAILURE WITH CONNECTING MEMCC");  }

  //get initial centroids  
  std::unordered_map<std::string, int> centroids;
  char* prev;
  uint32_t node;
  for(int i =0; i < NUM_CLUSTERS ; i++){
    string mem_key = "prev_" + to_string(i);
    int j = i % NUM_SERVERS;
    prev = memcached_get_by_key(memc, group_keys[j].c_str(), group_keys[j].size(), mem_key.c_str(), mem_key.size(),
                                  &value_length, &flags, &rt); 
    if(!prev) {  INFO("GET+INITIALIZED___MEMCACHED_ERROR_%d", rt); }
    centroids.insert({prev, i}); 
  }
  const char *isFirst = "isFirst";
  bool first[m->blocks.size()] = { false };
//



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
            mut.lock();
            rt = memcached_exist_by_key(memc, group_keys[mapHost].c_str(), group_keys[mapHost].size(), isFirst, strlen(isFirst)); 
            mut.unlock();
 
            velox::OutputCollection results;
	    //1st
	    if(rt != MEMCACHED_SUCCESS) {

  	      const string block_name = m->blocks[id].second;

              char* next_line = new char[MAP_MAX_LINE];
              bzero(next_line, MAP_MAX_LINE);
              char next_char = 0;
              int index = 0;

              do {
                next_char = (char)velox::lean_peek(md, (uint32_t)job_id, block_name);
                next_line[index] = next_char;

                if (next_line[index] == '\n') {
                  if (index == 0) continue;

                  std::string line(next_line);
		  memoryPoints[id].emplace_back(line);
                  bzero(next_line, MAP_MAX_LINE);
                  index = 0;
                }

                index++;
              } while (next_char != -1); 
              delete[] next_line;
	      first[id] = true;
	    }
            task_execution->map(memoryPoints[id], results, centroids);

            int pointNum = 0;
            double xSum = 0.0;
            double ySum = 0.0;
            double _x = 0.0, _y =0.0;
            char tmp[516];
            string avgNum;
            avgNum.reserve(516); 

            for (auto& pair : results) {
              auto& key = pair.first;
              auto& value = pair.second;
	      auto centNum = centroids.find(key)->second;
              uint32_t node = centNum % NUM_SERVERS;
	     
	      for(vector<string>::iterator iter = value.begin(); iter != value.end(); iter++){
                    string val(*iter);
                    sscanf(val.c_str(), "%lf,%lf", &_x, &_y);
                    xSum += _x;
                    ySum += _y;
                    pointNum++;
              }
              size_t total = snprintf(tmp, 516, "%lf,%lf,%d", (xSum/pointNum), (ySum/pointNum), pointNum); 
              avgNum.append(tmp,total);
              value.clear();
              value.push_back(to_string(centNum));
              value.push_back(avgNum);
              avgNum = "";
              pointNum = 0;
              xSum = 0.0; ySum=0.0;

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

          for (int i=0; i<network_size; i++) {
            auto it = kv_blocks->find(index);
            if (it != kv_blocks->end()) {
              KeyValueShuffle* kv = &(it->second);
              peer->insert_key_value(kv);
            }
          }
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
    int c_f = 0;
    if(rt != MEMCACHED_SUCCESS){
        for(int i = 0; i < m->blocks.size(); i++){
            if(first[i] == 1){ c_f+=1;  } //INFO("first[%d] == %d", i, first[i]); }
        if(c_f == m->blocks.size()){
            rt = memcached_set_by_key(memc, group_keys[mapHost].c_str(), group_keys[mapHost].size(), isFirst, strlen(isFirst), 
                    isFirst, strlen(isFirst), (time_t)0, (uint32_t)0);
            if(rt != MEMCACHED_SUCCESS) {  INFO("MAKE__FIRST___MEMCACHED_ERROR_%d", rt); }    
        }
      }
    }

    //task_execution->after_map(options);
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
int iter = 0;
std::unordered_multimap<std::string, void*> reduceNum[1];
bool Executor::run_reduce (messages::Task* task) {
  auto block_size = GET_INT("filesystem.block");

  auto network_size = GET_VEC_STR("network.nodes").size();
  Histogram boundaries(network_size, 100);
  boundaries.initialize();
  velox::model::metadata metadata;

  task_handler* task_execution = nullptr;

  if (task->lang == "C++") {
    task_execution = new task_cxx(task->library, task->func_name);
  } else {
//    task_execution = new task_python(task->func_body, task->before_map, task->after_map);
  }
  task_execution->setup(false);

  uint32_t total_size = 0;
  uint32_t num_keys = 0;
  queue<std::thread> threads;
  DirectoryMR directory;
  uint32_t reducer_slot = directory.select_number_of_reducers(task->job_id);
  mutex mut;
  DEBUG("LAunching reducer with %i threads", reducer_slot);
  for (int reducer_id = 0; reducer_id < (int)reducer_slot; reducer_id++) {

    if (threads.size() >= 1) {
      threads.front().join();
      threads.pop();
    }
//soojeong
    struct timeval s, e;
    char* hostname = new char[8];
    gethostname(hostname, 8);
    int rHost = hostname[7]-'0';
    rHost -= 1;
// 
 
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
      int count = 0;
      int centNum = 0;
      string::size_type sz;
      int p = 0;

      while (ireader.is_next_key()) {
        string key;
        ireader.get_next_key(key);

        mut.lock();
        DEBUG("PROCCESSING KEY %s", key.c_str());
        mut.unlock();

        velox::OutputCollection output;
        values.clear();
	if(key.size() > 2){
	  values.clear();
	  bool isCent = true;
        //TODO: make a function to get values at a time
          while (ireader.is_next_value()) {
            string value;
            ireader.get_next_value(value);

	    if(isCent){
	  	centNum = stoi(value); 
		count = centNum / NUM_SERVERS;
		reduceNum[count].clear();
		isCent = false;
	    } 
	    else{ 
		if(value.size() < 2){ continue; }
		values.push_back(value);
		reduceNum[count].insert({value, NULL});
		p++;
	    } 
	  }
	}
	else{
	  values.clear();

          //soojeong
          while (ireader.is_next_value()) {
            string value;
            ireader.get_next_value(value);
            values.push_back(value);
          }
        }
        DEBUG("RUNNING REDUCER %s", key.c_str());

        if(values.size() > 0) {
          try {
            task_execution->reduce(key, values);

          } catch (std::exception& e) {
            ERROR("Error in the executer: %s", e.what());
//            exit(EXIT_FAILURE);
          }
        } //else { 
          //INFO("REDUCER skipping a KEY");
      }
    }, reducer_id));
  }

  while (!threads.empty()) {
    threads.front().join();
    threads.pop();
  }

  iter++;
  peer->notify_task_leader(task->leader, task->job_id, "REDUCE");


  return true;
}
// }}}
} /* eclipse  */
