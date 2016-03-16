#include "peermr.h"
#include "../messages/message.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"

namespace eclipse {

void PeerMR::insert_idata(messages::IDataInfo *idata_info) {
  directory.insert_idata_metadata(*idata_info);
  logger->info("Saving to SQLite db");
}
void PeerMR::insert_igroup(messages::IGroupInfo *igroup_info) {
  directory.insert_igroup_metadata(*igroup_info);
  logger->info("Saving to SQLite db");
}
void PeerMR::insert_iblock(messages::IBlockInfo *iblock_info) {
  directory.insert_iblock_metadata(*iblock_info);
  logger->info("Saving to SQLite db");
}

}  // namespace eclipse
