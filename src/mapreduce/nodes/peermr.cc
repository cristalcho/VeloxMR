#include "peermr.h"
#include "../messages/boost_impl.hh"
#include "../executor.hh"
#include "../fs/iwriter.h"
#include <iostream>
#include <sstream>
#include <memory>

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
  auto leader_node = h(task->input_path) % 3;
  Reply reply;

  logger->info ("Executing map");
  Executor exec(this);

  if (exec.run_map(task, block))
    reply.message = "MAPDONE";
  else 
    reply.message = "MAPFAILED";

  if (leader_node == id) {

  } else {
    network->send (leader_node, &reply);
  }
}
// }}}
// process_map_file {{{
bool PeerMR::process_map_file (messages::Task* m) {
  auto file = m->input_path;
  FileInfo fi;
  fi.num_block = 0;

  directory.select_file_metadata(file, &fi);

  int num_blocks = fi.num_block;
  if (num_blocks == 0) return false;

  for (int i = 0; i< num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata (file, i, &bi);
    auto block_name = bi.block_name;
    auto hash_key = bi.block_hash_key;

    auto block_node = boundaries->get_index(hash_key);

    if (block_node == id) {
      request(hash_key, block_name, std::bind(
            &PeerMR::process_map_block, this, 
            std::placeholders::_1,
            std::placeholders::_2, m));

    } else {
      network->send (block_node, m);
    }
  }
  return true;
}
// }}}
//// process_map_dataset {{{
//void PeerMR::process_map_dataset (messages::Task* m) {
//  vec_str dataset = task.dataset;
//
//  for (auto& file : dataset) {
//    which_node = disk_boundaries->get_index(h(file));
//    if (which_node == id) {
//      process_map_file(m);
//
//    } else {
//      network->send (which_node, m);
//    }
//  }
//}
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
  const uint32_t job_id = msg->job_id_;
  auto it = iwriters_.find(job_id);
  if (it != iwriters_.end()) {
    it->second->finalize();
    iwriters_.erase(it);
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
}  // namespace eclipse