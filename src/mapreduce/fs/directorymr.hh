#pragma once
#include "../../fs/directory.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"

namespace eclipse {

class DirectoryMR: public Directory {
 public:
  void init_db();
  void insert_idata_metadata(IDataInsert idata_insert);
  void insert_igroup_metadata(IGroupInsert igroup_insert);
  void insert_iblock_metadata(IBlockInsert iblock_insert);
  void select_idata_metadata(uint32_t job_id, uint32_t map_id,
      IDataInfo *idata_info);
  void select_igroup_metadata(uint32_t job_id, uint32_t map_id,
      uint32_t reducer_id, IGroupInfo *igroup_info);
  void select_iblock_metadata(uint32_t job_id, uint32_t map_id,
      uint32_t reducer_id, uint32_t block_seq, IBlockInfo *iblock_info);


 protected:
  static int idata_callback(void *idata_info, int argc, char **argv,
      char **azColName);
  static int igroup_callback(void *igroup_info, int argc, char **argv,
      char **azColName);
  static int iblock_callback(void *iblock_info, int argc, char **argv,
      char **azColName);
};

}
