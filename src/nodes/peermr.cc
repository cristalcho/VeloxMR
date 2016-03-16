void PeerDFS::insert_idata(messages::IDataInfo* idata_info) {
  directory.insert_idata_metadata(*idata_info);
  logger->info ("Saving to SQLite db");
}
void PeerDFS::insert_igroup(messages::IGroupInfo* igroup_info) {
  directory.insert_igroup_metadata(*igroup_info);
  logger->info ("Saving to SQLite db");
}
void PeerDFS::insert_iblock(messages::IBlockInfo* iblock_info) {
  directory.insert_iblock_metadata(*iblock_info);
  logger->info ("Saving to SQLite db");
}
