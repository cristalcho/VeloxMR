#include "task_executor.hh"
#include "../messages/boost_impl.hh"
#include "common/hash.hh"
#include "common/histogram.hh"
#include "messages/idatalist.hh"
#include "messages/finish_shuffle.h"
#include "messages/key_value_shuffle.h"
#include "messages/nodes_shuffling.hh"
#include "messages/taskstatus.hh"
#include "messages/job.hh"
#include "messages/task.hh"
#include "executor.hh"
#include "fs/iwriter.h"
#include "client/dfs.hh"
#include <iostream>
#include <sstream>
#include <exception>
#include <utility>
#include <memory>
#include <vector>
#include <random>
#include <unistd.h>
#include <thread>
#include <future>
#include <mutex>


using namespace std;
using namespace velox;


namespace eclipse {
// Constructors {{{
TaskExecutor::TaskExecutor(network::ClientHandler* net) : Node() {
  network = net;
  directory.create_tables();
  network_size = GET_VEC_STR("network.nodes").size();
}
// }}}
// ------------- MAPREDUCE ROUTINES ------------------
// LEADER functions
// job_accept {{{
void TaskExecutor::job_accept(messages::Job* m, std::function<void(void)> fn) {
  Histogram boundaries(network_size, 100);
  boundaries.initialize();
  jobs_callback[m->job_id] = fn;
  tasks_remaining[m->job_id] = 0;

  INFO("JOB recieved");
  if (m->type == "MAP") {

    std::map<int, vector<pair<uint32_t, string>>>map_nodes;

    // Organize a map of blocks per each block_node
    for (auto& file : m->files) {
      model::metadata md = velox::get_metadata(file, VELOX_LOGICAL_GENERATE);

      for (size_t i = 0; i < md.blocks.size(); i++) {

        uint32_t hash_key_of_block = md.hash_keys[i];
        string block_name          = md.blocks[i];
        int which_node             = boundaries.get_index(hash_key_of_block);

        auto it = map_nodes.find(which_node);
        if (it == map_nodes.end() )
          it = map_nodes.insert({which_node, {}}).first;

        it->second.push_back({hash_key_of_block, block_name});
      }
    }

    INFO("JOB LEADER %i Processing %i tasks", id, map_nodes.size());
    // Schedule the tasks
    tasks_remaining[m->job_id] = map_nodes.size();
    for (auto& task_stub : map_nodes) {
      Task task;
      task.job_id = m->job_id;
      task.func_name = m->map_name;
      task.type = m->type;
      task.library = m->library;
      task.leader = id;
      task.func_body = m->func_body;
      task.lang = m->lang;
      task.blocks = task_stub.second;
      task.before_map = m->before_map;
      task.after_map = m->after_map;
      network->send(task_stub.first, &task);
    }
  } else if (m->type == "REDUCE") {
    schedule_reduce(m);
  }
}
// }}}
//  map_finish_notify {{{
void TaskExecutor::map_finish_notify(FinishMap* info) {
  INFO("FinishMap arrived, remaining=%lu", tasks_remaining[info->job_id] - 1);
  for (auto& node : info->nodes)
    nodes_shuffling.insert(node);

  if (--tasks_remaining[info->job_id] <= 0) {
    current_nodes_shuffling = nodes_shuffling.size();

    for (auto& node : nodes_shuffling) {
      INFO("Sending Finishing shuffle request to node %i", node);
      FinishShuffle fs;
      fs.job_id_ = info->job_id;
      network->send(node, &fs);
    }
  }
}
// }}}
//  TaskStatus {{{
void TaskExecutor::task_accept_status(TaskStatus* m) {
  INFO("TaskStatus arrived | JOBID: %u", m->job_id);

  if (m->type == "REDUCE") {
    INFO("Task remaining for job id:%lu = %i", m->job_id, tasks_remaining[m->job_id]);
    if (--tasks_remaining[m->job_id] <= 0) {
      jobs_callback[m->job_id]();

      // flush nodes_shuffling
      nodes_shuffling.clear();
      tasks_remaining.erase(m->job_id);
      jobs_callback.erase(m->job_id);
    }


  } else if (m->type == "MAP"){
    INFO("Task remaining for job id:%lu = %i", m->job_id, current_nodes_shuffling);
    if (--current_nodes_shuffling <= 0) {
      jobs_callback[m->job_id]();
      tasks_remaining.erase(m->job_id);
    }
  }
}
// }}}
// Shuffler funtions
//  shuffle_finish_notify {{{
 void TaskExecutor::shuffle_finish_notify(uint32_t job_id) {
  INFO("I got Finish shuffle jobid: %lu", job_id);

  //Make sure all the nodes have finished shuffling
  try {
    auto it = iwriters_.find(job_id);
    if (it != iwriters_.end()) {
      it->second->finalize();
      iwriters_.erase(it);
    }

  } catch (std::exception& e) {
    ERROR("Iwriter exception");
  }

  TaskStatus ts;
  ts.job_id = job_id;
  ts.type = "MAP";
  uint32_t leader = job_id % network_size;

  DEBUG("Sending task status to leader %u", leader);
  network->send(leader, &ts);
}
// }}}
//  key_value_store {{{
void TaskExecutor::key_value_store(KeyValueShuffle *kv) {
  INFO("KVshuffle KV_ID=%lu, ID=%i, DST=%i", kv->kv_id, id, kv->node_id);

  if ((int)kv->node_id == id) {

    std::thread([&, this] (KeyValueShuffle kv) {
        try {
          write_key_value(&kv);
        } catch (exception& e) {
          ERROR("Error in key_value_store routine ex:%s", e.what());
        }

        NodesShuffling fs;
        fs.job_id = kv.job_id_;
        fs.id = id;
        network->send(kv.origin_id, &fs);
    }, std::move(*kv)).detach();

  } else {
    DEBUG("Forwarding KVS to another node");
    network->send(kv->node_id, kv);
  }
}
// }}}
// write_key_value {{{
void TaskExecutor::write_key_value(messages::KeyValueShuffle *kv_shuffle) {

  const uint32_t job_id = kv_shuffle->job_id_;
  std::shared_ptr<IWriter_interface> iwriter;
  auto it = iwriters_.find(job_id);
  if (it == iwriters_.end()) {
    const uint32_t map_id = kv_shuffle->map_id_;
    iwriter = std::make_shared<IWriter>(job_id, map_id);
    iwriters_.insert({job_id, iwriter});
  } else {
    iwriter = it->second;
  }

  for (auto& pair : kv_shuffle->kv_pairs) {
    const std::string& key = pair.first;
    auto& values = pair.second;

    local_mut2.lock();
    for (auto& v : values) {
      iwriter->add_key_value(key, v);
    }
    local_mut2.unlock();
  }
}
// }}}
// ------------- MAP ROUTINES ------------------
//  task_accezpt {{{
 void TaskExecutor::task_accept(Task* m) {
  if (m->type == "MAP") {
    request_local_map(m);

  } else {
    request_local_reduce(m);
  }
}
// }}}
// request_local_map {{{
void TaskExecutor::request_local_map (messages::Task* task) {
  Task stask = *task;

  std::thread([&, this](Task task) {
        Executor exec(this);
        exec.run_map(&task);
      }, stask).detach();
}
// }}}
// notify_task_leader {{{
void TaskExecutor::notify_task_leader(int leader, uint32_t job_id, string type) {
  int leader_node = (int) leader;

  TaskStatus ts;
  ts.is_success = true;
  ts.job_id = job_id;
  ts.type = type;

  network->send(leader_node, &ts);
}
// }}}
// notify_map_is_finished {{{
void TaskExecutor::notify_map_is_finished(uint32_t job_id,
    std::vector<uint32_t> nodes) {

  FinishMap ts;
  ts.job_id = job_id;
  ts.nodes = nodes;
  INFO("FINISHED MAP ");

  local_mut.lock();
  tasker_remaining_job.insert({job_id, ts});
  local_mut.unlock();

  try_finish_map(job_id);
}
// }}}
// insert_key_value {{{
void TaskExecutor::insert_key_value(KeyValueShuffle *kv) {
  local_mut.lock();
  tasker_remaining_nodes_shuffling.insert(kv->node_id);
  local_mut.unlock();

  network->send(kv->node_id, kv);
}
// }}}
// try_finish_map {{{
void TaskExecutor::try_finish_map(uint32_t job_id) {
  local_mut.lock();
  if (tasker_remaining_job.find(job_id) != tasker_remaining_job.end()) {
    if (tasker_remaining_nodes_shuffling.empty()) {
      auto ts = tasker_remaining_job[job_id];
      tasker_remaining_job.erase(job_id);
      local_mut.unlock();

      uint32_t leader = job_id % network_size;
      network->send(leader, &ts);
      return;
    }
  }
  local_mut.unlock();
}
// }}}
// shuffle_is_done {{{
void TaskExecutor::shuffle_is_done(uint32_t job_id, uint32_t id) {
  local_mut.lock();
  auto itr = tasker_remaining_nodes_shuffling.find(id);
  if(itr!=tasker_remaining_nodes_shuffling.end()){
    tasker_remaining_nodes_shuffling.erase(itr);
  }
  local_mut.unlock();

  try_finish_map(job_id);
}
// }}}
// ------------- REDUCE ROUTINES ------------------
// schedule_reduce {{{
void TaskExecutor::schedule_reduce(messages::Job* m) {
  auto reduce_nodes = nodes_shuffling;
  tasks_remaining[m->job_id] = reduce_nodes.size();

  INFO("JOB LEADER %i Processing REDUCE %i jobs", id, reduce_nodes.size());

  if (velox::exists(m->file_output))
    velox::remove(m->file_output);

  velox::touch(m->file_output);

  for (auto which_node : reduce_nodes) {
    Task task;
    task.type = "REDUCE";
    task.job_id = m->job_id;
    task.func_name = m->reduce_name;
    task.library = m->library;
    task.leader = id;
    task.file_output = m->file_output;
    task.func_body = m->func_body;
    task.lang = m->lang;

    network->send(which_node, &task);
  }
}
// }}}
// request_local_reduce {{{
void TaskExecutor::request_local_reduce (messages::Task* m) {
  logger->info ("Executing reduce jobid:%lu", m->job_id);
  auto map_id = 0;
  auto job_id = m->job_id;

  IDataInfo di;
  di.map_id = map_id;
  di.job_id = job_id;
  di.num_reducer = 0;
  directory.select_idata_metadata(job_id, map_id, &di);

  if (di.num_reducer > 0) { //! Perform reduce operation
    std::async(std::launch::async, [&]() {
        logger->info("Performing reduce operation");
        Executor exec(this);
        Task copy_task = *m;
        exec.run_reduce(&copy_task);
        });
  }
}
// }}}
}  // namespace eclipse
