#include "remotemr.h"
#include "remotedfs.hh"
#include "peermr.h"
#include "router.hh"
#include <functional>
#include "../common/context.hh"
#include "../messages/message.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"

namespace eclipse {

RemoteMR::RemoteMR(Context &c): RemoteDFS(c), peer(c) {
  routing_table.insert({"IBlockInfo", std::bind(&RemoteMR::insert_idata,
      this, std::placeholders::_1)});
  routing_table.insert({"IGroupInfo", std::bind(&RemoteMR::insert_igroup,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInfo", std::bind(&RemoteMR::insert_iblock,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInfoRequest", std::bind(&RemoteMR::request_idata,
      this, std::placeholders::_1)});
  routing_table.insert({"IGroupInfoRequest", std::bind(&RemoteMR::request_igroup,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInfoRequest", std::bind(&RemoteMR::request_iblock,
      this, std::placeholders::_1)});
}
bool RemoteMR::establish() {
  peer.establish();
  Router::establish();
  return true;
}
void RemoteMR::insert_idata(messages::Message *msg) {
  auto idata_info = dynamic_cast<messages::IDataInsert*>(msg);
  logger->info("IDataInfo received.");
  peer.insert_idata(idata_info);
}
void RemoteMR::insert_igroup(messages::Message *msg) {
  auto igroup_info = dynamic_cast<messages::IGroupInsert*>(msg);
  logger->info("IGroupInfo received.");
  peer.insert_igroup(igroup_info);
}
void RemoteMR::insert_iblock(messages::Message *msg) {
  auto iblock_info = dynamic_cast<messages::IBlockInsert*>(msg);
  logger->info("IBlockInfo received.");
  peer.insert_iblock(iblock_info);
}
void RemoteMR::request_idata(messages::Message *msg) {
  auto idata_info_request = dynamic_cast<messages::IDataInfoRequest*>(msg);
  auto idata_info = peer.request_idata(idata_info_request);
  network->send(0, &idata_info);
}
void RemoteMR::request_igroup(messages::Message *msg) {
  auto igroup_info_request = dynamic_cast<messages::IGroupInfoRequest*>(msg);
  auto igroup_info = peer.request_igroup(igroup_info_request);
  network->send(0, &igroup_info);
}
void RemoteMR::request_iblock(messages::Message *msg) {
  auto iblock_info_request = dynamic_cast<messages::IBlockInfoRequest*>(msg);
  auto iblock_info = peer.request_iblock(iblock_info_request);
  network->send(0, &iblock_info);
}

}  // namespace eclipse
