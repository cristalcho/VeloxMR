#include "remotemr.h"
#include <functional>
#include "peermr.h"
#include "../../nodes/router.hh"
#include "../../nodes/remotedfs.hh"
#include "../../common/context.hh"
#include "../../messages/message.hh"
#include "../../messages/reply.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/key_value_shuffle.h"

namespace eclipse {

RemoteMR::RemoteMR(Context &c): RemoteDFS(c), peer(c) {
  routing_table.insert({"IBlockInsert", std::bind(&RemoteMR::insert_idata,
      this, std::placeholders::_1)});
  routing_table.insert({"IGroupInsert", std::bind(&RemoteMR::insert_igroup,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInsert", std::bind(&RemoteMR::insert_iblock,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInfoRequest", std::bind(&RemoteMR::request_idata,
      this, std::placeholders::_1)});
  routing_table.insert({"IGroupInfoRequest", std::bind(&RemoteMR::request_igroup,
      this, std::placeholders::_1)});
  routing_table.insert({"IBlockInfoRequest", std::bind(&RemoteMR::request_iblock,
      this, std::placeholders::_1)});
  routing_table.insert({"KeyValueShuffle", std::bind(&RemoteMR::shuffle, this,
      std::placeholders::_1)});
}
bool RemoteMR::establish() {
  peer.establish();
  Router::establish();
  return true;
}
void RemoteMR::insert_idata(messages::Message *msg) {
  auto idata_insert = dynamic_cast<messages::IDataInsert*>(msg);
  logger->info("IDataInsert received.");
  bool ret = peer.insert_idata(idata_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
void RemoteMR::insert_igroup(messages::Message *msg) {
  auto igroup_insert = dynamic_cast<messages::IGroupInsert*>(msg);
  logger->info("IGroupInsert received.");
  bool ret = peer.insert_igroup(igroup_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
void RemoteMR::insert_iblock(messages::Message *msg) {
  auto iblock_insert = dynamic_cast<messages::IBlockInsert*>(msg);
  logger->info("IBlockInsert received.");
  bool ret = peer.insert_iblock(iblock_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
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
void RemoteMR::shuffle(messages::Message *msg) {
  auto kv_msg = dynamic_cast<messages::KeyValueShuffle*>(msg);
  peer.receive_kv(kv_msg);
}

}  // namespace eclipse
