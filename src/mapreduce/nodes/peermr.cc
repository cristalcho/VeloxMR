#include "peermr.h"
#include <string>
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

namespace eclipse {

PeerMR::PeerMR(Context &context): PeerDFS(context) {
  directory.init_db();
}
PeerMR::~PeerMR() {
}
// void PeerMR::shuffle(std::string key, std::string value) {
//   int net_id = h(key) % size;
//   if (net_id == id) {
//     iwriter_.add_key_value(key, value);
//   }
// }
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
void PeerMR::receive_key_value(messages::KeyValueShuffle *key_value) {
  const uint32_t job_id = key_value->job_id_;
  std::shared_ptr<IWriter> iwriter;
  auto it = iwriters_.find(job_id);
  if (it == iwriters_.end()) {
    const uint32_t map_id = key_value->map_id_;
    iwriter = std::make_shared<IWriter>(job_id, map_id);
    iwriters_.emplace(job_id, iwriter);
  }
  else {
    iwriter = it->second;
  }
  const std::string &key = key_value->key_;
  const std::string &value = key_value->value_;
  iwriters_[job_id]->add_key_value(key, value);
}
void PeerMR::finalize_iwriter(messages::FinishShuffle *msg) {
  const uint32_t job_id = msg->job_id_;
  auto it = iwriters_find(job_id);
  if (it != iwriters_.end()) {
    it->second->finalize();
    iwriters_.erase(it);
  }
}

}  // namespace eclipse
