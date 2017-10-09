#pragma once
#include <vector>
#include "../../fileleader/directory.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../messages/idatalist.hh"

namespace eclipse {

class DirectoryMR: public Directory {
  public:
    void create_tables();
    void insert_idata_metadata(IDataInsert idata_insert);
    void insert_igroup_metadata(IGroupInsert igroup_insert);
    void insert_iblock_metadata(IBlockInsert iblock_insert);
    void select_idata_metadata(uint32_t job_id, uint32_t map_id,
        IDataInfo *idata_info);
    void select_all_idata_metadata(IDataList &idata_list);
    void select_igroup_metadata(uint32_t job_id, uint32_t map_id,
        uint32_t reducer_id, IGroupInfo *igroup_info);
    void select_iblock_metadata(uint32_t job_id, uint32_t map_id,
        uint32_t reducer_id, uint32_t block_seq, IBlockInfo *iblock_info);

    uint32_t select_number_of_reducers(uint32_t job_id);

  private:
    using Directory::query_exec_simple;
};

}
