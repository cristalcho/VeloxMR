#ifndef ECLIPSEMR_NODES_PEERMR_H_
#define ECLIPSEMR_NODES_PEERMR_H_
#include <string>
#include <unordered_map>
#include "../../nodes/peerdfs.hh"
#include "../fs/directorymr.hh"
#include "../fs/iwriter_interface.hh"
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
#include "../messages/task.hh"

namespace eclipse {

class PeerMR: public PeerDFS {
 public:
  PeerMR();
  ~PeerMR() = default;

  void on_read(messages::Message *msg, int) override;
  bool insert_idata(messages::IDataInsert *msg);
  bool insert_igroup(messages::IGroupInsert *msg);
  bool insert_iblock(messages::IBlockInsert *msg);
  IDataInfo request_idata(messages::IDataInfoRequest *idata_info_request);
  IGroupInfo request_igroup(messages::IGroupInfoRequest *igroup_info_request);
  IBlockInfo request_iblock(messages::IBlockInfoRequest *iblock_info_request);
  void write_key_value(messages::KeyValueShuffle *key_value);
  void receive_kv(messages::KeyValueShuffle *kv_shuffle);
  void process_map_block (std::string, std::string, messages::Task*);
  bool process_map_file (messages::Task*);
  template<typename T> void process(T);

 protected:
  uint32_t net_size_;
  DirectoryMR directory;
  std::unordered_map<uint32_t, std::shared_ptr<IWriter_interface>> iwriters_;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_PEERMR_H_
