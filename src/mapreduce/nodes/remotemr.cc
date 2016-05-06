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
using namespace messages;
namespace ph = std::placeholders;
using std::bind;

RemoteMR::RemoteMR() {
  auto& rt = routing_table;
  rt.insert({"IBlockInsert", bind(&RemoteMR::insert_idata, this, ph::_1)});
  rt.insert({"IGroupInsert", bind(&RemoteMR::insert_igroup, this, ph::_1)});
  rt.insert({"IBlockInsert", bind(&RemoteMR::insert_iblock, this, ph::_1)});
  rt.insert({"IBlockInfoRequest", bind(&RemoteMR::request_idata, this, ph::_1)});
  rt.insert({"IGroupInfoRequest", bind(&RemoteMR::request_igroup, this, ph::_1)});
  rt.insert({"IBlockInfoRequest", bind(&RemoteMR::request_iblock, this, ph::_1)});
  rt.insert({"KeyValueShuffle", bind(&RemoteMR::shuffle, this, ph::_1)});
  rt.insert({"Task", bind(&RemoteMR::map, this, ph::_1)});
}
bool RemoteMR::establish() {
  peer_dfs = new PeerMR(); 
  peer = dynamic_cast<PeerMR*>(peer_dfs);
  peer->establish();
  Router::establish();
  return true;
}
// map {{{
void RemoteMR::map (messages::Message* _m) {
  auto m = dynamic_cast<Task*>(_m);
  logger->info("Task received.");
  bool ret = peer->process_map_file(m);

  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
// }}}
void RemoteMR::insert_idata(messages::Message *msg) {
  auto idata_insert = dynamic_cast<messages::IDataInsert*>(msg);
  logger->info("IDataInsert received.");
  bool ret = peer->insert_idata(idata_insert);
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
  bool ret = peer->insert_igroup(igroup_insert);
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
  bool ret = peer->insert_iblock(iblock_insert);
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
  auto idata_info = peer->request_idata(idata_info_request);
  network->send(0, &idata_info);
}
void RemoteMR::request_igroup(messages::Message *msg) {
  auto igroup_info_request = dynamic_cast<messages::IGroupInfoRequest*>(msg);
  auto igroup_info = peer->request_igroup(igroup_info_request);
  network->send(0, &igroup_info);
}
void RemoteMR::request_iblock(messages::Message *msg) {
  auto iblock_info_request = dynamic_cast<messages::IBlockInfoRequest*>(msg);
  auto iblock_info = peer->request_iblock(iblock_info_request);
  network->send(0, &iblock_info);
}
void RemoteMR::shuffle(messages::Message *msg) {
  auto kv_msg = dynamic_cast<messages::KeyValueShuffle*>(msg);
  peer->receive_kv(kv_msg);
}

}  // namespace eclipse
