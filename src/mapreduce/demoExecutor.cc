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

#define ITERATIONS 2
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

static string group_keys[NUM_SERVERS] = {"dicl", "peach", "no", "dfs", "map", "hadoop", "pear", "memc", "apple"};
inline memcached_st *_get_memcached() { 
    const char *server = "--SERVER=dumbo051 --SERVER=dumbo052 --SERVER=dumbo053 --SERVER=dumbo054 --SERVER=dumbo055 --SERVER=dumbo056 --SERVER=dumbo057 --SERVER=dumbo058 --SERVER=dumbo059";
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
  mapHost -= 1;
//  if(mapHost == 9) mapHost -= 1;
  

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
//      INFO("i : %d, j: %d, prev: %s", i, j, prev);
      if(!prev) {  INFO("GET+INITIALIZED___MEMCACHED_ERROR_%d", rt); }
      centroids.insert({prev, i}); 
    }
    const char *isFirst = "isFirst";
//


  bool first[m->blocks.size()] = { false };
  
  try {

  for (size_t map_id = 0; map_id < m->blocks.size(); map_id++) {
    
    //  INFO("block_size: %d", m->blocks.size());
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
//            struct timeval mutexS, mutexE;
//            gettimeofday(&mutexS, NULL);
            mut.lock();
            rt = memcached_exist_by_key(memc, group_keys[mapHost].c_str(), group_keys[mapHost].size(), isFirst, strlen(isFirst)); 
            mut.unlock();
//            gettimeofday(&mutexE, NULL);
//            INFO("[MUTEX_TIME]: %ld", mutexE.tv_sec - mutexS.tv_sec);
                 
            //1st iteration
            if(rt != MEMCACHED_SUCCESS){ 
              struct timeval readDataS, readDataE; 
              gettimeofday(&readDataS, NULL);

              velox::OutputCollection results;
              const string block_name = m->blocks[id].second; 
              Local_io local_io;
              string input = local_io.read(block_name);
              istringstream ss (std::move(input));
            
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
              gettimeofday(&readDataE, NULL);
              INFO("[READ DATA FROM DISK]: %ld", readDataE.tv_sec - readDataS.tv_sec);

              // check is it first time? 
//              struct timeval setMemoryS, setMemoryE;
//              struct timeval sendMapS, sendMapE;
          //    gettimeofday(&sendMapS, NULL);
              first[id] = true;  
              int centNum = 0 ;
              int pointNum = 0;
              double xSum = 0.0;
              double ySum = 0.0;
              double _x = 0.0, _y =0.0;
              char tmp[516];
              string avgNum;
              avgNum.reserve(516);

              for (auto& pair : results) {
//                gettimeofday(&setMemoryS, NULL);
                
                auto& key = pair.first;
                auto& value = pair.second;
               //set which node to send
                auto centNum = centroids.find(key)->second;
               // value.insert(value.begin(), to_string(centNum));
                uint32_t node = centNum % NUM_SERVERS;

               //soojeong - store vector initial add to mapper memcached
                for(vector<string>::iterator iter = value.begin(); iter != value.end(); iter++){
                    string val(*iter);
             //       INFO("%s", val.c_str());
                    umaparray[centNum][id].insert({*iter, NULL});
                //    if(val != to_string(centNum)){
                        sscanf(val.c_str(), "%lf,%lf", &_x, &_y);
                        xSum += _x;
                        ySum += _y;
                        pointNum++;
                 //   }
                }
                //------------------------------
                size_t total = snprintf(tmp, 516, "%lf,%lf,%d", (xSum/pointNum), (ySum/pointNum), pointNum); 
                avgNum.append(tmp,total);
                value.clear();
                value.push_back(to_string(centNum));
                value.push_back(avgNum);
//                gettimeofday(&setMemoryE, NULL);
//                INFO("  [SET MEMORY]: %ld", setMemoryE.tv_sec - setMemoryS.tv_sec); 

                avgNum = "";
                pointNum = 0;
                xSum = 0.0; ySum=0.0;
                //collect other mappers datas
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
//              gettimeofday(&sendMapE, NULL);
//              INFO("1st Mapper: %ld", sendMapE.tv_sec - sendMapS.tv_sec);
            }
            else{
              INFO("Second ITERATION_ <dumbo05%d>, Thread_%d", mapHost+1, id);
              velox::OutputCollection add;
              velox::OutputCollection del;
        
              size_t val_len;
              uint32_t flags;
              double oriX[NUM_CLUSTERS] = {}; 
              double oriY[NUM_CLUSTERS] = {}; 
              int total[NUM_CLUSTERS] = {};

              //get va[stoi(key)] from memory not disk
              struct timeval readMemoryS, readMemoryE, s, e;
              gettimeofday(&readMemoryS, NULL);
              for(int i=0; i<NUM_CLUSTERS; i++){
                //gettimeofday(&s, NULL);
               // INFO("%d Size: %d", i, umaparray[i][id].size());
                for(auto& um : umaparray[i][id]){
                    std::string line(um.first);
                    if(line.size() < 2) continue;
                           
                    task_execution->map(line, i, add, del, options);
                }
//                for(unordered_map<std::string, void*>::iterator iter = umaparray[i][id].begin(); iter != umaparray[i][id].end(); ++iter){
  //              }
               // gettimeofday(&e, NULL);
               // INFO("2[READ]: %ld", e.tv_sec - s.tv_sec);
              }
              gettimeofday(&readMemoryE, NULL);
              INFO("2[MEMORY READ]: %ld", readMemoryE.tv_sec - readMemoryS.tv_sec);

              //updates memory
//              struct timeval updateMemoryS, updateMemoryE;
//              gettimeofday(&updateMemoryS, NULL);
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
//              gettimeofday(&updateMemoryE, NULL);
//              INFO("2[UPDATE_MEMORY]: %ld", updateMemoryE.tv_sec - updateMemoryS.tv_sec);
             
//              struct timeval files, filee;
//              gettimeofday(&files, NULL); 
              char tmp[516];
              string avgNum;
              avgNum.reserve(516);

              for(int i=0; i<NUM_CLUSTERS; i++){
                for(unordered_map<std::string, void*>::iterator iter = umaparray[i][id].begin(); iter != umaparray[i][id].end(); ++iter){
                    double x=0.0, y=0.0;
                 //   int clusterTotal = 0;
                   
                    std::string line(iter->first);
                    if(line.size() < 2) continue;
                    sscanf(line.c_str(), "%lf,%lf", &x, &y);
                    oriX[i] += x; oriY[i] += y;
                    total[i] += 1;
                }
 
                size_t iterAvg = snprintf(tmp, 516, "%lf,%lf,%d", (oriX[i]/total[i]), (oriY[i]/total[i]), total[i]);
                avgNum.append(tmp, iterAvg);
                string key = to_string(i);
                std::vector<std::string> v;
                v.clear();
                v.push_back(avgNum);

                auto& value = v;
                uint32_t node = i % NUM_SERVERS;
                avgNum = ""; total[i]=0; oriX[i]=0.0; oriY[i]=0.0;       
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
//              gettimeofday(&filee, NULL);
//              INFO("2nd Mapper: %ld", filee.tv_sec - files.tv_sec);
            }
          }
          mut.lock();
          for (auto it: *kv_blocks)
              keys_blocks.push_back(it.first);
          mut.unlock();
          
          for (int i=0; i < network_size; i++) {
            auto it = kv_blocks->find(i);
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
    rHost -= 1;
//    if(rHost == 9) rHost -= 1;

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
      //      INFO("%s", value.c_str());
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
    //    INFO("BEFORE_REDUCER: %ld", e.tv_sec - s.tv_sec);
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
   //     INFO("BEFORE_REDUCER2 %ld", e.tv_sec - s.tv_sec);
        }
  //      INFO("host: %d i>>number of point: %d", rHost+1, p);

        DEBUG("RUNNING REDUCER %s", key.c_str());

        if(values.size() > 0) {
          try {
            task_execution->reduce(key, values);

            //what is it?            
 /*           if(key.size() >2){
              string skey = "cent_" + to_string(centNum);
              uint64_t addr = (uint64_t)&reduceNum[count];
              string addr_str = to_string(addr);
            mut.lock();
              rt = memcached_set_by_key(memc2, group_keys[rHost].c_str(), group_keys[rHost].size(), skey.c_str(), skey.size(), 
                                       addr_str.c_str(), addr_str.size(), (time_t)0, (uint32_t)0);
            mut.unlock();
            } */

          } catch (std::exception& e) {
            ERROR("Error in the executer: %s", e.what());
//            exit(EXIT_FAILURE);
          }
        }// else 
//          INFO("REDUCER skipping a KEY");

      }
    }, reducer_id));
  }

  while (!threads.empty()) {
    threads.front().join();
    threads.pop();
  }
  iter++;
  peer->notify_task_leader(task->leader, task->job_id, "REDUCE");

//  if(iter == ITERATIONS){
//    peer->notice_the_end();
//    for(int i=0; i<2; i++){ 
//        INFO("reduce#: %d", reduceNum[0].size());
//        reduceNum[0].clear();
//    } 
//  }
  return true;
}
// }}}
} /* eclipse  */
