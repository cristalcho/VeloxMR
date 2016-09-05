#include "peermr.h"
#include "../messages/boost_impl.hh"
#include "../executor.hh"
#include "../fs/iwriter.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

namespace eclipse {
// Constructors {{{
PeerMR::PeerMR() {
  auto ip_list = context.settings.get<std::vector<std::string>>("network.nodes");
  net_size_ = ip_list.size();
  directory.init_db();
}
// }}}
// process_map_block {{{
void PeerMR::process_map_block (string ignoreme, string block, Task* task) {
  auto leader_node = h(task->input_path) % nodes.size();
  Reply reply;

  logger->info ("Executing map");
  Executor exec(this);

  if (exec.run_map(task, block))
    reply.message = "MAPDONE";
  else 
    reply.message = "MAPFAILED";

  remaining_maps--;
  if (remaining_maps == 0)
    notify_map_leader(task);
}
// }}}
// process_map_file {{{
bool PeerMR::process_map_file (messages::Task* m, std::function<void(void)> f) {
  task_callbacks[m->job_id] = f;

  if (is_leader(m->input_path)) 
    map_leader(m);
  else
    map_follower(m);

  return true;
}
// }}}
// process KeyValueShuffle {{{
template<> void PeerMR::process(KeyValueShuffle *kv_shuffle) {
  auto key = kv_shuffle->key_;
  int hash_value = h(key) % net_size_;
  uint32_t dst_net_id = hash_value;
  std::stringstream ss;

  ss << "hash_value: " << hash_value;
  ss << "key: " << key << " net_id: " << id << " net_size: " <<
    net_size_ << " dst: " << dst_net_id;

  logger->info(ss.str().c_str());

  if (dst_net_id == id) {
    write_key_value(kv_shuffle);

  } else {
    logger->debug ("!!! Another node");
    network->send(dst_net_id, kv_shuffle);
  }
}
// }}}
// process FinishShuffle {{{
template<> void PeerMR::process(FinishShuffle *msg) {
  logger->info (" I got Finish shuffle");

  //Make sure all the nodes have finished shuffling
    try { 
      const uint32_t job_id = msg->job_id_;
      auto it = iwriters_.find(job_id);
      if (it != iwriters_.end()) {
        it->second->finalize();
        iwriters_.erase(it);
      }
    } catch (std::exception& e) {
      logger->error ("Iwriter exception");
    }
    if (task_callbacks.find(msg->job_id_) != task_callbacks.end())
      task_callbacks[msg->job_id_]();
}
// }}}
// process Task {{{
template<> void PeerMR::process(Task* m) {
  if (m->get_type_task() == "MAP") {
    map_follower(m);
  
  } else {
    auto map_id = m->map_id;
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
  }
}
// }}}
// process TaskStatus {{{
template<> void PeerMR::process(messages::TaskStatus* m) {
  logger->info ("I got a Task status: %d  jobid: %u", m->is_success, m->job_id);
  if (m->is_success) {
    remaining_follower_map_nodes--;
  }

  if (remaining_follower_map_nodes == 0) {
    finish_map(m->job_id);
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

  } else {
    PeerDFS::on_read(msg, 0);
  }
}
// }}}
// insert_idata {{{
bool PeerMR::insert_idata(messages::IDataInsert *msg) {
  directory.insert_idata_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
// }}}
// insert_igroup {{{
bool PeerMR::insert_igroup(messages::IGroupInsert *msg) {
  directory.insert_igroup_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
// }}}
// insert_iblock {{{
bool PeerMR::insert_iblock(messages::IBlockInsert *msg) {
  directory.insert_iblock_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
// }}}
// request_idata {{{
IDataInfo PeerMR::request_idata(messages::IDataInfoRequest *idata_info_request) {
  IDataInfo idata_info;
  directory.select_idata_metadata(idata_info_request->job_id,
      idata_info_request->map_id, &idata_info);
  return idata_info;
}
// }}}
// request_idata {{{
IDataList PeerMR::request_idata_list() {
  IDataList output;
  directory.select_all_idata_metadata(output);
  return output;
}
// }}}
// request_igroup {{{
IGroupInfo PeerMR::request_igroup(messages::IGroupInfoRequest *igroup_info_request) {
  IGroupInfo igroup_info;
  directory.select_igroup_metadata(igroup_info_request->job_id,
      igroup_info_request->map_id, igroup_info_request->reducer_id,
      &igroup_info);
  return igroup_info;
}
// }}}
// request_iblock {{{
IBlockInfo PeerMR::request_iblock(messages::IBlockInfoRequest *iblock_info_request) {
  IBlockInfo iblock_info;
  directory.select_iblock_metadata(iblock_info_request->job_id,
      iblock_info_request->map_id, iblock_info_request->reducer_id,
      iblock_info_request->block_seq, &iblock_info);
  return iblock_info;
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
  const std::string &key = kv_shuffle->key_;
  const std::string &value = kv_shuffle->value_;
  iwriters_[job_id]->add_key_value(key, value);
}
// }}}
// receive_kv {{{
void PeerMR::receive_kv(messages::KeyValueShuffle *kv_shuffle) {
  process(kv_shuffle);
}
// }}}
// finish_map {{{
void PeerMR::finish_map (int job_id_) {
    FinishShuffle fs;
    fs.job_id_ = job_id_;

    for (uint8_t i = 0; i < net_size_; i++) {
      if (i != id) {
        network->send(i, &fs);
      }
    }
    process(&fs);
}
// }}}
// process_reduce {{{
bool PeerMR::process_reduce (messages::Task* m) {
  for (uint8_t i = 0; i < net_size_; i++) {
    if (i != id) {
      network->send(i, m);
    }
  }
  process(m);
}
// }}}
// map_leader {{{
void PeerMR::map_leader (messages::Task* m) {
  auto file = m->input_path;
  FileInfo fi;
  fi.num_block = 0;

  directory.select_file_metadata(file, &fi);

  int num_blocks = fi.num_block;
  if (num_blocks == 0) return;

  map<int, Task> tasks;
  for (int i = 0; i< num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata (file, i, &bi);
    auto block_name = bi.block_name;
    auto hash_key = bi.block_hash_key;

    auto block_node = boundaries->get_index(hash_key);
    tasks.insert({block_node, *m});
    tasks[block_node].blocks.push_back({hash_key, block_name});
  }
  remaining_follower_map_nodes = tasks.size();
  logger->info ("%d nodes will run maps", remaining_follower_map_nodes);

  for (auto& task : tasks) {
    if (task.first == id) {
      map_follower(&task.second);
    
    } else {
      logger->info ("Forwaring Map task to %d jobid:%" PRIu32, task.first, m->job_id);
      network->send (task.first, &task.second);
    }
  }
}
// }}}
// map_follower {{{
void PeerMR::map_follower (messages::Task* m) {
  logger->info ("Executing map jobid:%d", m->job_id);
  remaining_maps = m->blocks.size();
  for (auto& block : m->blocks)
      request(block.first, block.second, std::bind(
            &PeerMR::process_map_block, this, 
            std::placeholders::_1,
            std::placeholders::_2, m));

}
// }}}
// format {{{
bool PeerMR::format () {
  PeerDFS::format();
  directory.init_db();
}
// }}}
// is_leader {{{
bool PeerMR::is_leader(std::string f) {
  return (id == (h(f) % nodes.size()));
}
// }}}
// notify_map_leader {{{
void PeerMR::notify_map_leader (messages::Task* m) {
  auto leader_node = h(m->input_path) % nodes.size();

  TaskStatus ts;
  ts.is_success = true;
  ts.job_id = m->job_id;

  if (leader_node == id) {
    process(&ts);

  } else {
    network->send(leader_node, &ts);
  }
}
// }}}
}  // namespace eclipse
