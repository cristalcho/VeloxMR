#ifndef ECLIPSEMR_NODES_PEERMR_H_
#define ECLIPSEMR_NODES_PEERMR_H_
#include "peerdfs.hh"
#include "../fs/directorymr.hh"
#include "../messages/message.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"

namespace eclipse {

class PeerMR: public PeerDFS {
 public:
  PeerMR(Context &context);
  ~PeerMR();

  bool insert_idata(messages::IDataInsert *msg);
  bool insert_igroup(messages::IGroupInsert *msg);
  bool insert_iblock(messages::IBlockInsert *msg);
  IDataInfo request_idata(messages::IDataInfoRequest *idata_info_request);
  IGroupInfo request_igroup(messages::IGroupInfoRequest *igroup_info_request);
  IBlockInfo request_iblock(messages::IBlockInfoRequest *iblock_info_request);

 protected:
  DirectoryMR directory;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_PEERMR_H_
