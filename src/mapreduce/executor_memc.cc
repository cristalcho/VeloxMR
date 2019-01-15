#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/histogram.hh"
#include "../common/hash.hh"
#include "../mapreduce/output_collection.hh"
#include "../mapreduce/fs/ireader.h"
#include "messages/key_value_shuffle.h"
#include "../client/dfs.hh"
#include "task_cxx.hh"
//#include "task_python.hh"
//soojeong
#include <libmemcached/memcached.h>
//

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
#include <algorithm>
#include <time.h>

#define ITERATIONS 5
#define NUM_SERVERS 9 //7
#define NUM_CLUSTERS 9
#define MAP_MAX_LINE 10000
using namespace eclipse;
using namespace std;

namespace eclipse {

// Constructor {{{
Executor::Executor(TaskExecutor* p, std::unordered_multimap<std::string, void*>** uma) : peer(p), umaparray(uma) {}
Executor::~Executor() {}
// }}}

//static string group_keys[NUM_SERVERS] = {"pear", "peach", "no", "cat", "dog", "yes", "apple"};
static string group_keys[NUM_SERVERS] = {"dicl", "peach", "no", "dfs", "map", "hadoop", "pear", "memc", "apple"};
inline memcached_st *_get_memcached() { 
    const char *server = "--SERVER=dumbo070 --SERVER=dumbo071 --SERVER=dumbo072 --SERVER=dumbo073 --SERVER=dumbo074 --SERVER=dumbo075 --SERVER=dumbo076 --SERVER=dumbo077 --SERVER=dumbo079";
    return memcached(server, strlen(server));
}

// run_map {{{
bool Executor::run_map (messages::Task* m) {

  std::unordered_map<std::string, void*> options;
  vector<uint32_t> keys_blocks;
  queue<std::thread> threads;
  mutex mut;

  auto network_size = GET_VEC_STR("network.nodes").size();
  size_t mappers = GET_INT("mapreduce.mappers");

  task_handler* task_execution = nullptr;

  //where do mapper running?
  const int len = 256;
  char* hostname = new char[len];
  gethostname(hostname, len);
  int mapHost = hostname[7]-'0';
  if(mapHost == 9) mapHost -= 1;
  

  if (m->lang == "C++") {
    task_execution = new task_cxx(m->library, m->func_name);
  } else {
//    task_execution = new task_python(m->func_body, m->before_map, m->after_map);
  }
  task_execution->setup(true);

  INFO("Launching mapper with %i threads", m->blocks.size());
  
  try {
    task_execution->before_map(options);
  } catch(std::exception& e) {
    INFO("exception in before_map: %s", e.what());
  }

  DEBUG("before_map is finished");

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
      INFO("i : %d, j: %d, prev: %s", i, j, prev);
      if(!prev) {  INFO("GET+INITIALIZED___MEMCACHED_ERROR_%d", rt); }
      centroids.insert({prev, i}); 
    }
    const char *isFirst = "isFirst";
//


  bool first[m->blocks.size()] = { false };
  
  try {

  for (size_t map_id = 0; map_id < m->blocks.size(); map_id++) {
    
      INFO("block_size: %d", m->blocks.size());
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
                  
         //   if(memcached_exist_by_key(memc, group_keys[mapHost].c_str(), group_keys[mapHost].size(), isFirst, strlen(isFirst)) != MEMCACHED_SUCCESS){        
              if(rt != MEMCACHED_SUCCESS){ // && first[id]==0){
              //1st iteration
      //        INFO("First ITERATION_ <dumbo07%d>, Thread_%d", mapHost+1, id);
              velox::OutputCollection results;
              const string block_name = m->blocks[id].second; 
              Local_io local_io;
              string input = local_io.read(block_name);
              istringstream ss (std::move(input));
            
              char* next_line = new char[MAP_MAX_LINE];
            
       //       int t = 0;        
              struct timeval files, filee;
              gettimeofday(&files, NULL);
              while (!ss.eof()) {
                bzero(next_line, MAP_MAX_LINE);
                ss.getline (next_line, MAP_MAX_LINE);
                if (strnlen(next_line, MAP_MAX_LINE) == 0) 
                  continue;
                std::string line(next_line);
                task_execution->map(line, results, options);
         //       t++;
              }
              gettimeofday(&filee, NULL);
              INFO("DISKREAD: %ld", filee.tv_sec - files.tv_sec);
//              INFO(">>MapHost: %d, id: %d, map total: %d", mapHost, id, t);
              delete[] next_line;
              // check is it first time? 
              first[id] = true; 
 
              int centNum = 0 ;
              for (auto& pair : results) {
           //     int c_p =0; 
                auto& key = pair.first;
                auto& value = pair.second;
               //set which node to send
                auto centNum = centroids.find(key)->second;
                value.insert(value.begin(), to_string(centNum));
                uint32_t node = centNum % NUM_SERVERS;

               //soojeong - store vector initial add to mapper memcached
                for(vector<string>::iterator iter = value.begin(); iter != value.end(); iter++){
                    string val(*iter);
                    umaparray[centNum][id].insert({*iter, NULL});
                    //INFO("[[[cent: %d, value: %s", centNum, val.c_str()); 
          //          c_p++;
                }
                //------------------------------

                //collect other mappers datas
              gettimeofday(&files, NULL);
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
              gettimeofday(&filee, NULL);
              INFO("shuffle: %ld", filee.tv_sec - files.tv_sec);
            }
            else{
              velox::OutputCollection add;
              velox::OutputCollection del;
        
              //INFO("Second ITERATION_ <dumbo07%d>, Thread_%d", mapHost+1, id);
              size_t val_len;
              uint32_t flags;

        //get va[stoi(key)] from memcached not disk
              struct timeval start, end;
              for(int i=0; i<NUM_CLUSTERS; i++){
              gettimeofday(&start, NULL);
                for(unordered_map<std::string, void*>::iterator iter = umaparray[i][id].begin(); iter != umaparray[i][id].end(); ++iter){
                    std::string line(iter->first);
                    if(line.size() < 2) continue;
                    task_execution->map(line, i, add, del, options);
                }
              gettimeofday(&end, NULL);
              INFO("MEMORYREAD_2nd: %ld", end.tv_sec - start.tv_sec);
              }

              gettimeofday(&start, NULL);
              for(auto& pair : del){
                auto& k = pair.first;
                auto& v = pair.second;
                for(vector<string>::iterator it=v.begin(); it!=v.end(); ++it){
                    umaparray[stoi(k)][id].erase(*it);
                    std::string line(*it);
                }
              }
        
              for(auto& pair : add){
                auto& k = pair.first;
                auto& v = pair.second;
                for(vector<string>::iterator it=v.begin(); it!=v.end(); ++it){
                  umaparray[stoi(k)][id].insert({*it, NULL});
                  std::string line(*it);
                }
              }
              gettimeofday(&end, NULL);
              INFO("MEMORY_AFFECT: %ld", end.tv_sec - start.tv_sec);
              

       //merge add and delete outputcollection
              gettimeofday(&start, NULL);
              for(auto& pair : add){
                auto& key = pair.first;
                auto& value = pair.second;
                value.insert(value.begin(), "add");
                bool addKey = false;
                for(auto& p : del){
                  auto& k = p.first;
                  auto& v = p.second;
                  v.insert(v.begin(), "del");
                  if( k == key ){
                    v.insert(v.end(), value.begin(), value.end());

                    addKey = true;
                    break; 
                  }
                }
                if(!addKey){
                  for(vector<string>::iterator iter=value.begin(); iter!=value.end(); ++iter){
                      del.insert(key, *iter);
                  }
                }
              }
              gettimeofday(&end, NULL);
              INFO("MAKERESULTS: %ld", end.tv_sec - start.tv_sec);
              
              long erase;
            //struct timeval start, end;
              gettimeofday(&start, NULL);
              for(auto& pair : del){
                auto& key = pair.first;
                auto& value = pair.second;
                uint32_t node = (stoi(key)) % NUM_SERVERS;
            
                auto it =kv_blocks->find(node);
                if (it == kv_blocks->end()) {
                  it = kv_blocks->insert({node, {}}).first;
                
                  it->second.node_id = node;
                  it->second.job_id_ = m->job_id;
                  it->second.map_id_ = 0;
                  it->second.origin_id = context.id;
                }
                it->second.kv_pairs.insert({key, std::move(value)});
              }
              gettimeofday(&end, NULL);
              INFO("SHUFFLES: %ld", end.tv_sec - start.tv_sec);
            }
          }
          mut.lock();
          for (auto it: *kv_blocks)
              keys_blocks.push_back(it.first);
          mut.unlock();
          
          struct timeval s, e;
          gettimeofday(&s, NULL);
          for (int i=0; i < network_size; i++) {
            auto it = kv_blocks->find(i);
            if (it != kv_blocks->end()) {
              KeyValueShuffle* kv = &(it->second);
              peer->insert_key_value(kv);
            }
          }
          delete kv_blocks;
          gettimeofday(&e, NULL);
          INFO("Shuffling: %ld", e.tv_sec - s.tv_sec);
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
//  mut.lock();
    for(int i = 0; i < m->blocks.size(); i++){
        if(first[i] == 1){ c_f+=1;  } //INFO("first[%d] == %d", i, first[i]); }
        if(c_f == m->blocks.size()){
            rt = memcached_set_by_key(memc, group_keys[mapHost].c_str(), group_keys[mapHost].size(), isFirst, strlen(isFirst), 
                    isFirst, strlen(isFirst), (time_t)0, (uint32_t)0);
//            INFO("YEAH ALL FIRST ELEMENT IS FIRST LETS GO TO THE SECOND ITERATION");
            if(rt != MEMCACHED_SUCCESS) {  INFO("MAKE__FIRST___MEMCACHED_ERROR_%d", rt); }    
        }
    }
//            mut.unlock();
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
int iter = 0;
std::unordered_multimap<std::string, void*> reduceNum[1];
bool Executor::run_reduce (messages::Task* task) {
  //auto reducer_slot = GET_INT("mapreduce.reduce_slot");
 

  auto network_size = GET_VEC_STR("network.nodes").size();
  Histogram boundaries(network_size, 100);
  boundaries.initialize();
  velox::model::metadata metadata;

  task_handler* task_execution = nullptr;

  if (task->lang == "C++") {
    task_execution = new task_cxx(task->library, task->func_name);
  } else {
//    task_execution = new task_python(task->func_bode, task->before_map, task->after_map);
  }
  task_execution->setup(false);

  queue<std::thread> threads;
  DirectoryMR directory;
  uint32_t reducer_slot = directory.select_number_of_reducers(task->job_id);
  mutex mut;
  DEBUG("LAunching reducer with %i threads", reducer_slot);
  for (int reducer_id = 0; reducer_id < reducer_slot; reducer_id++) {
    if (threads.size() >= 1) {
      threads.front().join();
      threads.pop();
    }

//soojeong
    struct timeval s, e;
    char* hostname = new char[8];
    gethostname(hostname, 8);
    int rHost = hostname[7]-'0';
    if(rHost == 9) rHost -= 1;

    size_t value_length;
    uint32_t flags;
    memcached_return_t rt;
    mut.lock();
    memcached_st *memc2 = _get_memcached();
    mut.unlock();
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
        DEBUG("PROCCESSING KEY %s, %d", key.c_str(), key.size());
        mut.unlock();

        if(key.size() > 2){
          values.clear();

          //TODO: make a function to get values at a time
          gettimeofday(&s, NULL);
          bool isCent = true;
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
        gettimeofday(&e, NULL);
        INFO("BEFORE_REDUCER: %ld", e.tv_sec - s.tv_sec);
        }//2nd iteration~
        else{
          values.clear();

          //TODO: make a function to get values at a time
          //soojeong
          gettimeofday(&s, NULL);
          while (ireader.is_next_value()) {
            string value;
            ireader.get_next_value(value);
            values.push_back(value);
          }
        gettimeofday(&e, NULL);
        INFO("BEFORE_REDUCER2 %ld", e.tv_sec - s.tv_sec);
        }
//        INFO("host: %d i>>number of point: %d", rHost+1, p);

        DEBUG("RUNNING REDUCER %s", key.c_str());

        if(values.size() > 0) {
          try {
            task_execution->reduce(key, values);
            
            if(key.size() >2){
              string skey = "cent_" + to_string(centNum);
              uint64_t addr = (uint64_t)&reduceNum[count];
              string addr_str = to_string(addr);
            mut.lock();
              rt = memcached_set_by_key(memc2, group_keys[rHost].c_str(), group_keys[rHost].size(), skey.c_str(), skey.size(), 
                                       addr_str.c_str(), addr_str.size(), (time_t)0, (uint32_t)0);
            mut.unlock();
            }

          } catch (std::exception& e) {
            ERROR("Error in the executer: %s", e.what());
            exit(EXIT_FAILURE);
          }
        } else 
          INFO("REDUCER skipping a KEY");

      }
    }, reducer_id));
  }

  while (!threads.empty()) {
    threads.front().join();
    threads.pop();
  }
  iter++;
  peer->notify_task_leader(task->leader, task->job_id, "REDUCE");

  if(iter == ITERATIONS){
    peer->notice_the_end();
//    for(int i=0; i<2; i++){ 
        INFO("reduce#: %d", reduceNum[0].size());
        reduceNum[0].clear();
//    } 
  }
  return true;
}
// }}}
} /* eclipse  */
