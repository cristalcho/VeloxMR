#pragma once
#include "../../nodes/remotedfs.hh"
#include "../../messages/message.hh"
#include "peermr.h"

namespace eclipse {

class RemoteMR: public RemoteDFS {
 public: 
   RemoteMR (PeerMR*, network::Network*);

  void insert_idata(messages::Message*);
  void insert_igroup(messages::Message*);
  void insert_iblock(messages::Message*);
  void request_idata(messages::Message*);
  void request_igroup(messages::Message*);
  void request_iblock(messages::Message*);
  void shuffle(messages::Message*);
  void map(messages::Message*);
  void reply_map(messages::Message*);

 protected:
  PeerMR* peer;
};

}  // namespace eclipse
