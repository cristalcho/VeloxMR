#ifndef ECLIPSEMR_NODES_REMOTEMR_H_
#define ECLIPSEMR_NODES_REMOTEMR_H_
#include "remotedfs.hh"
#include "peermr.h"
#include "../messages/message.hh"

namespace eclipse {

class RemoteMR: public RemoteDFS {
 public: 
  RemoteMR(Context &c);

  bool establish();
  void insert_idata(messages::Message *msg);
  void insert_igroup(messages::Message *msg);
  void insert_iblock(messages::Message *msg);

 private:
  PeerMR peer;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_REMOTEMR_H_
