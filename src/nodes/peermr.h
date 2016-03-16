#ifndef ECLIPSEMR_NODES_PEERMR_H_
#define ECLIPSEMR_NODES_PEERMR_H_
#include "peerdfs.hh"
#include "../messages/message.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"

namespace eclipse {

class PeerMR: public PeerDFS {
 public:
  PeerMR();
  ~PeerMR();

  void insert_idata(messages::IDataInfo *idata_info);
  void insert_igroup(messages::IGroupInfo *igroup_info);
  void insert_iblock(messages::IBlockInfo *iblock_info);
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_PEERMR_H_
