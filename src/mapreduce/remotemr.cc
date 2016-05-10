#include "remotemr.hh"
#include "../messages/boost_impl.hh"
#include <functional>

namespace eclipse {
using namespace messages;
namespace ph = std::placeholders;
using std::bind;

// Constructor {{{
RemoteMR::RemoteMR(Context &c): RemoteDFS(c) {
  auto& rt = routing_table;
  rt.insert({"IBlockInsert", bind(&RemoteMR::insert_idata, this, ph::_1)});
  rt.insert({"IGroupInsert", bind(&RemoteMR::insert_igroup, this, ph::_1)});
  rt.insert({"IBlockInsert", bind(&RemoteMR::insert_iblock, this, ph::_1)});
  rt.insert({"IBlockInfoRequest", bind(&RemoteMR::request_idata, this, ph::_1)});
  rt.insert({"IGroupInfoRequest", bind(&RemoteMR::request_igroup, this, ph::_1)});
  rt.insert({"IBlockInfoRequest", bind(&RemoteMR::request_iblock, this, ph::_1)});
  rt.insert({"Task", bind(&RemoteMR::map, this, ph::_1)});
}
// }}}
// establish {{{
bool RemoteMR::establish() {
  peer  = make_unique<PeerMR> (context);
  peer_mr = dynamic_cast<PeerMR*> (peer.get());
  peer_mr->establish();
  Router::establish();
  return true;
}
// }}}
// map {{{
void RemoteMR::map (messages::Message* _m) {
  auto m = dynamic_cast<Task*>(_m);
  logger->info("Task received.");
  bool ret = peer_mr->process_map_file(m);

  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
// }}}
// insert_idata {{{
void RemoteMR::insert_idata(Message *msg) {
  auto idata_insert = dynamic_cast<IDataInsert*>(msg);
  logger->info("IDataInsert received.");
  bool ret = peer_mr->insert_idata(idata_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
// }}}
// insert_iblock {{{
void RemoteMR::insert_igroup(Message *msg) {
  auto igroup_insert = dynamic_cast<IGroupInsert*>(msg);
  logger->info("IGroupInsert received.");
  bool ret = peer_mr->insert_igroup(igroup_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
// }}}
// insert_iblock {{{
void RemoteMR::insert_iblock(Message *msg) {
  auto iblock_insert = dynamic_cast<IBlockInsert*>(msg);
  logger->info("IBlockInsert received.");
  bool ret = peer_mr->insert_iblock(iblock_insert);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(0, &reply);
}
// }}}
// request_idata  {{{
void RemoteMR::request_idata(Message *msg) {
  auto idata_info_request = dynamic_cast<IDataInfoRequest*>(msg);
  auto idata_info = peer_mr->request_idata(idata_info_request);
  network->send(0, &idata_info);
}
// }}}
// request_igroup {{{
void RemoteMR::request_igroup(Message *msg) {
  auto igroup_info_request = dynamic_cast<IGroupInfoRequest*>(msg);
  auto igroup_info = peer_mr->request_igroup(igroup_info_request);
  network->send(0, &igroup_info);
}
// }}}
// request_iblock {{{
void RemoteMR::request_iblock(Message *msg) {
  auto iblock_info_request = dynamic_cast<IBlockInfoRequest*>(msg);
  auto iblock_info = peer_mr->request_iblock(iblock_info_request);
  network->send(0, &iblock_info);
}
// }}}

}  // namespace eclipse
