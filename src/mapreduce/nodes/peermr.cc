//
//
//
//
//
//
//
//
//
#include "peermr.h"
#include "../messages/boost_impl.hh"
#include "../executor.hh"
#include "../py_executor.hh"
#include "../fs/iwriter.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <random>
#include <unistd.h>

static int idebug = 0;

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
// request_idata {{{
IDataList PeerMR::request_idata_list() {
  IDataList output;
  directory.select_all_idata_metadata(output);
  return output;
}
// }}}
// is_leader {{{
bool PeerMR::is_leader(std::string f) {
  return (id == (int)(h(f) % network_size));
}
// }}}
// ------------- MAPREDUCE ROUTINES ------------------
// process FinishMap {{{
template<> void PeerMR::process(FinishMap *m) {
  current_nodes_shuffling++;

  if (current_nodes_shuffling >= nodes_shuffling[m->job_id].size() && subjobs_remaining[m->job_id] <= 0) {
    jobs_callback[m->job_id]();
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

  FinishMap fm;
  fm.job_id = m->job_id_;
  uint32_t leader = m->job_id_ % network_size;

  if ((int32_t)leader == id)
    process(&fm);
  else 
    network->send(leader, &fm);
}
// }}}
// process KeyValueShuffle {{{
template<> void PeerMR::process(KeyValueShuffle *kv_shuffle) {
  auto key = kv_shuffle->key_;
  int which_node = h(key) % network_size;

  DEBUG("KVshuffle H=%lu, K=%s, ID=%i, DST=%i", h(key),
      key.c_str(), id, which_node);

  
  shuffled_nodes[kv_shuffle->job_id_].insert(which_node);

  if (which_node == id) {
    if (kv_shuffle->is_header){
      keys_to_be_recv += kv_shuffle->number_of_keys;
      return;
    }
    write_key_value(kv_shuffle);

    current_keys++;
    if (current_keys >= keys_to_be_recv){
      DEBUG("i got all my keys");
      current_keys = 0;
      keys_to_be_recv = 0;
      FinishShuffle fs;
      fs.job_id_ = kv_shuffle->job_id_;
      process(&fs);
    }

  } else {
    DEBUG("Forwarding KVS to another node");
    network->send(which_node, kv_shuffle);
  }
}
// }}}
// process NodesShuffling {{{
template<> void PeerMR::process(NodesShuffling* m) {
  DEBUG("I got a list of %i keys from map jobid:%lu", m->nodes.size(), m->job_id);

  if (nodes_shuffling.find(m->job_id) == nodes_shuffling.end()) {
    nodes_shuffling[m->job_id] = vector<int> (m->nodes.begin(), m->nodes.end());
  } else {
    //std::copy(.end(), m->nodes.begin(), m->nodes.end());
    std::copy(m->nodes.begin(), m->nodes.end(), back_inserter(nodes_shuffling[m->job_id]));
  }

  // Remote repeated elements on the list
  auto& v = nodes_shuffling[m->job_id];
  std::sort(v.begin(), v.end());
  auto last = std::unique(v.begin(), v.end());
  v.erase(last, v.end()); 
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

  DEBUG("Subjob remaining : %i current nodes:%i",  subjobs_remaining[m->job_id], current_nodes_shuffling);
  if (subjobs_remaining[m->job_id] <= 0) {
    if (current_nodes_shuffling >= nodes_shuffling[m->job_id].size() && m->type == "MAP") {
      DEBUG("REturning to client");
      jobs_callback[m->job_id]();
      //current_nodes_shuffling = nodes_shuffling[m->job_id].size();
      //for (auto node : nodes_shuffling[m->job_id]) {
      //  FinishShuffle fs;
      //  fs.job_id_ = m->job_id;
      //  if (node == id)
      //    process(&fs);
      //  else
      //    network->send(node, &fs);
      //}
    } else if (m->type == "REDUCE") {
      jobs_callback[m->job_id]();
      idebug =0;
    }
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
    sjob_status.type = m->type;

    int which_node = m->job_id % network_size;
    if (which_node == id)
      process(&sjob_status);
    else
      network->send(which_node, &sjob_status);
  }
}
// }}}
// process FileInfo {{{
template<> void PeerMR::process (FileInfo* m) {
  PeerDFS::process(m);
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

  } else if (type == "FinishMap") {
    auto task_ = dynamic_cast<FinishMap*>(msg);
    process(task_);

  } else if (type == "NodesShuffling") {
    auto task_ = dynamic_cast<NodesShuffling*>(msg);
    process(task_);

  } else {
    PeerDFS::on_read(msg, 0);
  }
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
      sjob.func_body = m->func_body;
      sjob.lang = m->lang;

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
// ------------- MAP ROUTINES ------------------
// schedule_map {{{
void PeerMR::schedule_map(messages::SubJob* m) {
  INFO("File leader %i schedules a map task", id);
  auto file = m->file;
  FileInfo fi;
  fi.num_block = 0;

  directory.select_file_metadata(file, &fi);
  current_nodes_shuffling = 0;
  nodes_shuffling[m->job_id] = std::vector<int> ();

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
    task.func_body = m->func_body;
    task.lang = m->lang;

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
// request_local_map {{{
void PeerMR::request_local_map (messages::Task* m) {
  auto disk_path = GET_STR("path.scratch");
  logger->info ("Executing map subjobid:%lu", m->subjob_id);
  for (auto& block : m->blocks) {
    logger->info ("Executing map on block: %s", block.second.c_str());
      
      //Spin-lock until the block has arrived to the node (iterative workflow patch)
      string path = disk_path + "/" + block.second;
      while(access(path.c_str(), F_OK) == -1);

      request(block.first, block.second, std::bind(
            &PeerMR::run_map_onto_block, this,
            std::placeholders::_1,
            std::placeholders::_2, m));
  }

}
// }}}
// run_map_onto_block {{{
void PeerMR::run_map_onto_block(string ignoreme, string block, Task* stask) {
  Reply reply;

  INFO("Executing map");
  if (stask->lang == "C++") {
    Executor exec(this);

    if (exec.run_map(stask, block))
      reply.message = "MAPDONE";
    else
      reply.message = "MAPFAILED";

  } else if (stask->lang == "Python") {
    PYexecutor exec(this);

    if (exec.run_map(stask, block))
      reply.message = "MAPDONE";
    else
      reply.message = "MAPFAILED";
  }

  auto job_nodes = shuffled_nodes[stask->job_id];
  NodesShuffling ns;
  ns.job_id = stask->job_id;
  ns.nodes = std::vector<int>(job_nodes.begin(), job_nodes.end());

  auto which_node = stask->job_id % network_size;
  if ((int)which_node == id)
    process(&ns);
  else
    network->send(which_node, &ns);

  //sleep(10);
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
// write_key_value {{{
void PeerMR::write_key_value(messages::KeyValueShuffle *kv_shuffle) {

  const uint32_t job_id = kv_shuffle->job_id_;
  std::shared_ptr<IWriter_interface> iwriter;
  auto it = iwriters_.find(job_id);
  if (it == iwriters_.end()) {
    const uint32_t map_id = kv_shuffle->map_id_;
    //iwriter = std::make_shared<IWriter>(job_id, map_id);
    iwriters_.insert({job_id, std::make_shared<IWriter>(job_id, map_id)});
  }
  else {
    iwriter = it->second;
  }
  const std::string& key = kv_shuffle->key_;
  auto& values = kv_shuffle->value_;
  DEBUG("PEERDFS passed %i keys", idebug++);
  for (auto& v : values) {
    iwriters_[job_id]->add_key_value(key, v);
  }
}
// }}}
// ------------- REDUCE ROUTINES ------------------
// schedule_reduce {{{
void PeerMR::schedule_reduce(messages::Job* m) {
  subjobs_remaining[m->job_id] = 1;
  auto reduce_nodes = nodes_shuffling[m->job_id];

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
    task.func_body = m->func_body;
    task.lang = m->lang;

    if (which_node == id)
      process(&task);
    else
      network->send(which_node, &task);
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
    if (m->lang == "C++") {
      Executor exec(this);
      Reply reply;

      if (exec.run_reduce(m))
        reply.message = "MAPDONE";
      else
        reply.message = "MAPFAILED";

    } else if (m->lang == "Python") {
      PYexecutor exec(this);
      Reply reply;

      if (exec.run_reduce(m))
        reply.message = "MAPDONE";
      else
        reply.message = "MAPFAILED";
    }
  }

  notify_task_leader(m->leader, m->job_id, m->job_id, "REDUCE");
}
// }}}
// ------------- REDUCE OUTPUT ROUTINES ------------------
// submit_block {{{
void PeerMR::submit_block(messages::BlockInfo* m) {
  auto file_name = m->file_name;
  int which_node = h(file_name) % network_size; 

  if (which_node == id) { //If it is its leader node
    insert_block(m);
  } else {
    network->send(which_node, m);
  }
}
// }}}
// insert_file {{{
bool PeerMR::insert_file(messages::FileInfo* f) {
  //bool ret = directory.file_exist(f->name.c_str());

  messages::FileInfo sel_fi;
  directory.select_file_metadata(f->name.c_str(), &sel_fi);
  bool ret = (sel_fi.is_valid && strcmp(sel_fi.name.c_str(), f->name.c_str()) == 0);

  if (ret) {
    FileUpdate fu;
    fu.name = f->name;
    fu.size = f->size + sel_fi.size;
    fu.num_block = f->num_block + sel_fi.num_block;
    directory.update_file_metadata(fu);
    replicate_metadata();

    INFO("File:%s exists in db, Updated to (%u, %u)", fu.name.c_str(), fu.size, fu.num_block);
    
    return false;
  }

  directory.insert_file_metadata(*f);
  replicate_metadata();

  logger->info("Saving to SQLite db");
  return true;
}
// }}}
// insert_block {{{
bool PeerMR::insert_block(messages::BlockInfo* m) {
  // A block from other nodes should be last in the sequence.
  messages::BlockInfo last_block;
  directory.select_last_block_metadata(m->file_name.c_str(), &last_block);
  if(last_block.is_valid)
    m->seq = last_block.seq + 1;

  return PeerDFS::insert_block(m);
}
// }}}
}  // namespace eclipse
