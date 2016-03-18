#include "peermr.h"
#include "../messages/message.hh"
#include "../messages/boost_impl_mr.hh"
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
void PeerMR::insert_idata(messages::IDataInsert *msg) {
  directory.insert_idata_metadata(*msg);
  logger->info("Saving to SQLite db");
}
void PeerMR::insert_igroup(messages::IGroupInsert *msg) {
  directory.insert_igroup_metadata(*msg);
  logger->info("Saving to SQLite db");
}
void PeerMR::insert_iblock(messages::IBlockInsert *msg) {
  directory.insert_iblock_metadata(*msg);
  logger->info("Saving to SQLite db");
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

}  // namespace eclipse
