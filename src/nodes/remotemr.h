#ifndef ECLIPSEMR_NODES_REMOTEMR_H_
#define ECLIPSEMR_NODES_REMOTEMR_H_
#include "remotedfs.hh"
#include "peermr.h"
#include "../messages/message.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/boost_impl_mr.hh"

namespace eclipse {

class RemoteMR: public RemoteDFS {
 public: 
  RemoteMR(Context &c);

  bool establish();
  void insert_idata(messages::Message *msg);
  void insert_igroup(messages::Message *msg);
  void insert_iblock(messages::Message *msg);
  void request_idata(messages::Message *msg);
  void request_igroup(messages::Message *msg);
  void request_iblock(messages::Message *msg);

 protected:
  PeerMR peer;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_REMOTEMR_H_
