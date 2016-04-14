#include "peermr.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include "../messages/message.hh"
#include "../messages/boost_impl.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/key_value_shuffle.h"
#include "../messages/finish_shuffle.h"
#include "../../common/hash.hh"

namespace eclipse {

PeerMR::PeerMR() {
  auto ip_list = context.settings.get<std::vector<std::string>>("network.nodes");
  for (uint32_t i = 0; i < ip_list.size(); ++i) {
    if (ip_of_this == ip_list[i]) {
      net_id_ = i;
      break;
    }
  }
  net_size_ = ip_list.size();
}
PeerMR::~PeerMR() {
}
template<> void PeerMR::process(KeyValueShuffle *kv_shuffle) {
  auto key = kv_shuffle->key_;
  int hash_value = h(key) % net_size_;
  std::stringstream ss;
  ss << "hash_value: " << hash_value << std::endl;
  logger->info(ss.str().c_str());
  // int dst_net_id = boundaries->get_index(hash_value);
  uint32_t dst_net_id = hash_value;
  std::stringstream ss1;
  ss1 << "key: " << key << " net_id: " << net_id_ << " net_size: " <<
    net_size_ << " dst: " << dst_net_id << std::endl;
  logger->info(ss1.str().c_str());
  if (dst_net_id == net_id_) {
    write_key_value(kv_shuffle);
  } else {
    std::cout << "!!! Another node" << std::endl;
    network->send(dst_net_id, kv_shuffle);
  }
}
template<> void PeerMR::process(FinishShuffle *msg) {
  const uint32_t job_id = msg->job_id_;
  auto it = iwriters_.find(job_id);
  if (it != iwriters_.end()) {
    it->second->finalize();
    iwriters_.erase(it);
  }
}
void PeerMR::on_read(messages::Message *msg) {
  std::string type = msg->get_type();
  if (type == "KeyValueShuffle") {
    auto kv_shuffle = dynamic_cast<KeyValueShuffle*>(msg);
    process(kv_shuffle);
  } else if (type == "FinishShuffle") {
    auto finish_shuffle = dynamic_cast<FinishShuffle*>(msg);
    process(finish_shuffle);
  } else {
    PeerDFS::on_read(msg);
  }
}
bool PeerMR::insert_idata(messages::IDataInsert *msg) {
  directory.insert_idata_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
bool PeerMR::insert_igroup(messages::IGroupInsert *msg) {
  directory.insert_igroup_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
bool PeerMR::insert_iblock(messages::IBlockInsert *msg) {
  directory.insert_iblock_metadata(*msg);
  logger->info("Saving to SQLite db");
  return true;
}
IDataInfo PeerMR::request_idata(messages::IDataInfoRequest
    *idata_info_request) {
  IDataInfo idata_info;
  directory.select_idata_metadata(idata_info_request->job_id,
      idata_info_request->map_id, &idata_info);
  return idata_info;
}
IGroupInfo PeerMR::request_igroup(messages::IGroupInfoRequest
    *igroup_info_request) {
  IGroupInfo igroup_info;
  directory.select_igroup_metadata(igroup_info_request->job_id,
      igroup_info_request->map_id, igroup_info_request->reducer_id,
      &igroup_info);
  return igroup_info;
}
IBlockInfo PeerMR::request_iblock(messages::IBlockInfoRequest
    *iblock_info_request) {
  IBlockInfo iblock_info;
  directory.select_iblock_metadata(iblock_info_request->job_id,
      iblock_info_request->map_id, iblock_info_request->reducer_id,
      iblock_info_request->block_seq, &iblock_info);
  return iblock_info;
}
void PeerMR::write_key_value(messages::KeyValueShuffle *kv_shuffle) {
  const uint32_t job_id = kv_shuffle->job_id_;
  std::shared_ptr<IWriter> iwriter;
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
void PeerMR::receive_kv(messages::KeyValueShuffle *kv_shuffle) {
  process(kv_shuffle);
}

}  // namespace eclipse
