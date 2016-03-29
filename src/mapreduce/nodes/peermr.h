#ifndef ECLIPSEMR_NODES_PEERMR_H_
#define ECLIPSEMR_NODES_PEERMR_H_
#include "peerdfs.hh"
#include <string>
#include <unordered_map>
#include "../fs/directorymr.hh"
#include "../fs/iwriter.h"
#include "../../messages/message.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/key_value_shuffle.h"
#include "../messages/finish_shuffle.h"

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
  void receive_key_value(messages::KeyValueShuffle *key_value);
  void finalize_iwriter(messages::FinishShuffle *msg);

 protected:
  DirectoryMR directory;
  std::unorderd_map<uint32_t, std::shared_ptr<IWriter>> iwriters_;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_PEERMR_H_
