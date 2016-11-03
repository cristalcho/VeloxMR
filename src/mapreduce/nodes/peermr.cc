#include "peermr.h"
#include "../messages/boost_impl.hh"
#include "../executor.hh"
#include "../fs/iwriter.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <random>

namespace eclipse {
// Constructors {{{
PeerMR::PeerMR(network::Network* net) : PeerDFS(net) {
  directory.open_db();
}
// }}}
// format {{{
bool PeerMR::format () {
  PeerDFS::format();
  directory.init_db();
  return true;
}
// }}}
// process KeyValueShuffle {{{
template<> void PeerMR::process(KeyValueShuffle *kv_shuffle) {
  auto key = kv_shuffle->key_;
  int which_node = h(key) % network_size;

  DEBUG("KVshuffle H=%lu, K=%s, ID=%i, DST=%i", h(key),
      key.c_str(), id, which_node);

  shuffled_nodes[kv_shuffle->job_id_].insert(which_node);
  shuffled_keys[kv_shuffle->job_id_].insert(key);

  if (which_node == id) {
    write_key_value(kv_shuffle);

  } else {
    DEBUG("Forwarding KVS to another node");
    network->send(which_node, kv_shuffle);
  }
}
// }}}
// process FinishShuffle {{{
template<> void PeerMR::process(FinishShuffle *m) {
  DEBUG("I got Finish shuffle jobid: %lu", m->job_id_);

  //Make sure all the nodes have finished shuffling
  try {
    const uint32_t job_id = m->job_id_;
    auto it = iwriters_.find(job_id);
    if (it != iwriters_.end()) {
      it->second->finalize();
      iwriters_.erase(it);
    }
  } catch (std::exception& e) {
    ERROR("Iwriter exception");
  }
}
// }}}
// process SubJob{{{
template<> void PeerMR::process(messages::SubJob* m) {
  if (m->type == "MAP") {
    schedule_map(m);
  }
}
// }}}
// process SubJobStatus {{{
template<> void PeerMR::process(messages::SubJobStatus* m) {
  INFO("I got a SubJob status: %d  jobid: %u", m->is_success, m->job_id);
  if (m->is_success) {
    subjobs_remaining[m->job_id]--;
  }

  DEBUG("Subjob remaining : %i",  subjobs_remaining[m->job_id]);
  if (subjobs_remaining[m->job_id] <= 0) {
    jobs_callback[m->job_id]();
  }
}
// }}}
// process Task {{{
template<> void PeerMR::process(Task* m) {
  if (m->type == "MAP") {
    request_local_map(m);

  } else {
    request_local_reduce(m);
  }
}
// }}}
// process TaskStatus {{{
template<> void PeerMR::process(TaskStatus* m) {
  INFO("I got a TaskStatus: %d  jobid: %u", m->is_success, m->job_id);
  if (m->is_success) {
    tasks_remaining[m->subjob_id]--;
  }

  INFO("Task remaining for job id:%lu = %i", m->job_id, tasks_remaining[m->job_id]);
  if (tasks_remaining[m->subjob_id] == 0) {

    SubJobStatus sjob_status;
    sjob_status.job_id = m->job_id;
    sjob_status.is_success = true;

    int which_node = m->job_id % network_size;
    if (which_node == id)
      process(&sjob_status);
    else
      network->send(which_node, &sjob_status);
  }
}
// }}}
// process IDataKeys {{{
template<> void PeerMR::process(IDataKeys* m) {
  DEBUG("I got a list of %i keys from map", m->keys.size());
  if (stored_idata.find(m->job_id) == stored_idata.end()) {
    stored_idata[m->job_id] = vector<string> (m->keys.begin(), m->keys.end());
  } else {
    std::copy(stored_idata[m->job_id].end(), m->keys.begin(), m->keys.end());
  }
}
// }}}
// on_read {{{
void PeerMR::on_read(messages::Message *msg, int) {
  std::string type = msg->get_type();

  if (type == "KeyValueShuffle") {
    auto kv_shuffle = dynamic_cast<KeyValueShuffle*>(msg);
    process(kv_shuffle);

  } else if (type == "FinishShuffle") {
    auto finish_shuffle = dynamic_cast<FinishShuffle*>(msg);
    process(finish_shuffle);

  } else if (type == "Task") {
    auto task_ = dynamic_cast<Task*>(msg);
    process(task_);

  } else if (type == "TaskStatus") {
    auto task_ = dynamic_cast<TaskStatus*>(msg);
    process(task_);

  } else if (type == "SubJob") {
    auto task_ = dynamic_cast<SubJob*>(msg);
    process(task_);

  } else if (type == "SubJobStatus") {
    auto task_ = dynamic_cast<SubJobStatus*>(msg);
    process(task_);

  } else if (type == "IDataKeys") {
    auto task_ = dynamic_cast<IDataKeys*>(msg);
    process(task_);

  } else {
    PeerDFS::on_read(msg, 0);
  }
}
// }}}
// request_idata {{{
IDataList PeerMR::request_idata_list() {
  IDataList output;
  directory.select_all_idata_metadata(output);
  return output;
}
// }}}
// write_key_value {{{
void PeerMR::write_key_value(messages::KeyValueShuffle *kv_shuffle) {
  const uint32_t job_id = kv_shuffle->job_id_;
  std::shared_ptr<IWriter_interface> iwriter;
  auto it = iwriters_.find(job_id);
  if (it == iwriters_.end()) {
    const uint32_t map_id = kv_shuffle->map_id_;
    iwriter = std::make_shared<IWriter>(job_id, map_id);
    iwriters_.emplace(job_id, iwriter);
  }
  else {
    iwriter = it->second;
  }
  const std::string& key = kv_shuffle->key_;
  const std::string& value = kv_shuffle->value_;
  iwriters_[job_id]->add_key_value(key, value);
}
// }}}
// request_save_idata {{{
void PeerMR::request_save_idata (int job_id_) {
    FinishShuffle fs;
    fs.job_id_ = job_id_;

    for (uint8_t i = 0; i < network_size; i++) {
      if (i != id) {
        network->send(i, &fs);
      }
    }
    process(&fs);
}
// }}}
// request_local_map {{{
void PeerMR::request_local_map (messages::Task* m) {
  logger->info ("Executing map subjobid:%lu", m->subjob_id);
  for (auto& block : m->blocks) {
    logger->info ("Executing map on block: %s", block.second.c_str());
      request(block.first, block.second, std::bind(
            &PeerMR::run_map_onto_block, this,
            std::placeholders::_1,
            std::placeholders::_2, m));
  }

}
// }}}
// request_local_reduce {{{
void PeerMR::request_local_reduce (messages::Task* m) {
  logger->info ("Executing reduce jobid:%lu", m->job_id);
  auto map_id = 0;
  auto job_id = m->job_id;

  IDataInfo di;
  di.map_id = map_id;
  di.job_id = job_id;
  di.num_reducer = 0;
  directory.select_idata_metadata(job_id, map_id, &di);

  if (di.num_reducer > 0) { //! Perform reduce operation
    logger->info("Performing reduce operation");
    Executor exec(this);
    Reply reply;

    if (exec.run_reduce(m))
      reply.message = "MAPDONE";
    else
      reply.message = "MAPFAILED";
  }

  notify_task_leader(m->leader, m->job_id, m->job_id, "REDUCE");
}
// }}}
// is_leader {{{
bool PeerMR::is_leader(std::string f) {
  return (id == (int)(h(f) % network_size));
}
// }}}
// process_job {{{
bool PeerMR::process_job (messages::Job* m, std::function<void(void)> f) {
  jobs_callback[m->job_id] = f;

  if (m->type == "MAP") {
    subjobs_remaining[m->job_id] = m->files.size();
    logger->info("JOB LEADER %i Processing %i jobs", id, m->files.size());

    for (auto& file : m->files) {
      int which_node = h(file) % network_size;

      SubJob sjob;
      sjob.type = "MAP";
      sjob.library = m->library;
      sjob.map_name = m->map_name;
      sjob.reduce_name = m->reduce_name;
      sjob.file = file;
      sjob.job_id = m->job_id;

      if (which_node == id)
        process(&sjob);
      else
        network->send(which_node, &sjob);
    }
  } else if (m->type == "REDUCE") {
    schedule_reduce(m);
  }
  return true;
}
// }}}
// run_map_onto_block {{{
void PeerMR::run_map_onto_block(string ignoreme, string block, Task* stask) {
  Reply reply;

  INFO("Executing map");
  Executor exec(this);

  if (exec.run_map(stask, block))
    reply.message = "MAPDONE";
  else
    reply.message = "MAPFAILED";

  auto job_keys = shuffled_keys[stask->job_id];
  IDataKeys idata;
  idata.job_id = stask->job_id;
  idata.keys.push_back(""); //= vector<string>;
  copy(job_keys.begin(), job_keys.end(), idata.keys.begin());

  auto which_node = stask->job_id % network_size;
  if ((int)which_node == id)
    process(&idata);
  else
    network->send(which_node, &idata);

  for (auto which_node: shuffled_nodes[stask->job_id]) {
    FinishShuffle fs;
    fs.job_id_ = stask->job_id;
    if (which_node == id)
      process(&fs);
    else
      network->send(which_node, &fs);
  }
  notify_task_leader (stask->leader, stask->subjob_id, stask->job_id, "MAP");
}
// }}}
// notify_task_leader {{{
void PeerMR::notify_task_leader(int leader, uint32_t subjob_id, uint32_t job_id, string type) {
  int leader_node = (int) leader;

  TaskStatus ts;
  ts.is_success = true;
  ts.job_id = job_id;
  ts.subjob_id = subjob_id;
  ts.type = type;

  if (leader_node == id) {
    process(&ts);

  } else {
    network->send(leader_node, &ts);
  }
}
// }}}
// schedule_map {{{
void PeerMR::schedule_map(messages::SubJob* m) {
  INFO("File leader %i schedules a map task", id);
  auto file = m->file;
  FileInfo fi;
  fi.num_block = 0;

  directory.select_file_metadata(file, &fi);

  int num_blocks = fi.num_block;
  if (num_blocks == 0) return;  //! Not file found in the db


  // Generate random subjob id
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 
      std::numeric_limits<uint32_t>::max());

  uint32_t subjob_id = dist(rng);

  map<int, Task> tasks;
  for (int i = 0; i < num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata(file, i, &bi);
    auto block_name = bi.name;
    auto hash_key = bi.hash_key;
    auto block_node = boundaries->get_index(hash_key);

    Task task;
    task.job_id = m->job_id;
    task.subjob_id = subjob_id;
    task.func_name = m->map_name;
    task.type = m->type;
    task.library = m->library;
    task.input_path = m->file;
    task.leader = id;

    tasks.insert({block_node, task});
    tasks[block_node].blocks.push_back({hash_key, block_name});
  }

  tasks_remaining[subjob_id] = tasks.size();
  INFO("%d nodes will run maps", tasks.size());

  for (auto& task : tasks) {
    if (task.first == id) {
      request_local_map(&task.second);

    } else {
      INFO("Forwaring Map task to %d jobid:%" PRIu32, task.first, m->job_id);
      network->send (task.first, &task.second);
    }
  }
}
// }}}
// schedule_reduce {{{
void PeerMR::schedule_reduce(messages::Job* m) {
  subjobs_remaining[m->job_id] = 1;

  set<int> reduce_nodes;
  for (auto key : stored_idata[m->job_id]) {
    uint32_t hash = h(key) % network_size;
    reduce_nodes.insert(hash);
  }

  tasks_remaining[m->job_id] = reduce_nodes.size();
  logger->info("JOB LEADER %i Processing REDUCE %i jobs", id, reduce_nodes.size());
  for (auto which_node : reduce_nodes) {
    Task task;
    task.type = "REDUCE";
    task.job_id = m->job_id;
    task.func_name = m->reduce_name;
    task.library = m->library;
    task.leader = id;
    task.file_output = m->file_output;

    if (which_node == id)
      process(&task);
    else
      network->send(which_node, &task);
  }
}
// }}}
}  // namespace eclipse
