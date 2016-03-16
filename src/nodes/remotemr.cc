


void RemoteDFS::insert_idata(messages::Message* msg) {
  auto idata_info = dynamic_cast<messages::IDataInfo*>(msg);
  logger->info("IDataInfo received.");
  peer.insert_idata(idata_info);
}
void RemoteDFS::insert_igroup(messages::Message* msg) {
  auto igroup_info = dynamic_cast<messages::IGroupInfo*>(msg);
  logger->info("IGroupInfo received.");
  peer.insert_igroup(igroup_info);
}
void RemoteDFS::insert_iblock(messages::Message* msg) {
  auto iblock_info = dynamic_cast<messages::IBlockInfo*>(msg);
  logger->info("IBlockInfo received.");
  peer.insert_iblock(iblock_info);
}
