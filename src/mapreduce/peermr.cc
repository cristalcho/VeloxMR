#include "peermr.hh"
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
#include "executor.hh"

namespace eclipse {

PeerMR::PeerMR(Context &context): PeerDFS(context) {
  directory.init_db();
}
PeerMR::~PeerMR() {
}

// process_map_block {{{
void PeerMR::process_map_block (string ignoreme, string block, Task* task) {
  auto leader_node = h(task->input_path) % 3;
  Reply reply;

  logger->info ("Executing map");
//  //Executor exec(this);
//  
//    if (exec.run_map(task, block))
//      reply.message = "MAPDONE";
//    else 
//      reply.message = "MAPFAILED";

  network->send (leader_node, &reply);
}
// }}}
// process_map_file {{{
bool PeerMR::process_map_file (messages::Task* m) {
  auto file = m->input_path;
  FileInfo fi;
  fi.num_block = 0;

  directory.select_file_metadata(file, &fi);

  int num_blocks = fi.num_block;
  if (num_blocks == 0) return false;

  for (int i = 0; i< num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata (file, i, &bi);
    auto block_name = bi.block_name;
    auto hash_key = bi.block_hash_key;

    auto block_node = boundaries->get_index(hash_key);

    if (block_node == id) {
      request(hash_key, block_name, std::bind(
            &PeerMR::process_map_block, this, 
            std::placeholders::_1,
            std::placeholders::_2, m));

    } else {
      network->send (block_node, m);
    }
  }
  return true;
}
// }}}
//// process_map_dataset {{{
//void PeerMR::process_map_dataset (messages::Task* m) {
//  vec_str dataset = task.dataset;
//
//  for (auto& file : dataset) {
//    which_node = disk_boundaries->get_index(h(file));
//    if (which_node == id) {
//      process_map_file(m);
//
//    } else {
//      network->send (which_node, m);
//    }
//  }
//}
// }}}
// {{{
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
// }}}
}  // namespace eclipse
