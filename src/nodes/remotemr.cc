#include "remotemr.h"
#include "remotedfs.hh"
#include "router.hh"
#include <functional>
#include "../common/context.hh"
#include "../messages/message.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"

namespace eclipse {

RemoteMR::RemoteMR(Context &c): RemoteDFS(c){
  routing_table.insert({"IBlockInfo", std::bind(&RemoteMR::insert_idata, this,
      std::placeholders::_1)});
  routing_table.insert({"IGroupInfo", std::bind(&RemoteMR::insert_igroup, this,
      std::placeholders::_1)});
  routing_table.insert({"IBlockInfo", std::bind(&RemoteMR::insert_iblock, this,
      std::placeholders::_1)});
}
bool RemoteMR::establish() {
  peer.establish();
  Router::establish();
  return true;
}
void RemoteMR::insert_idata(messages::Message *msg) {
  auto idata_info = dynamic_cast<messages::IDataInfo*>(msg);
  logger->info("IDataInfo received.");
  peer.insert_idata(idata_info);
}
void RemoteMR::insert_igroup(messages::Message *msg) {
  auto igroup_info = dynamic_cast<messages::IGroupInfo*>(msg);
  logger->info("IGroupInfo received.");
  peer.insert_igroup(igroup_info);
}
void RemoteMR::insert_iblock(messages::Message *msg) {
  auto iblock_info = dynamic_cast<messages::IBlockInfo*>(msg);
  logger->info("IBlockInfo received.");
  peer.insert_iblock(iblock_info);
}

}  // namespace eclipse
