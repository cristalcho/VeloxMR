#pragma once
#include "peermr.hh"
#include "../nodes/remotedfs.hh"
#include "../messages/message.hh"

namespace eclipse {

class RemoteMR: public RemoteDFS {
 public: 
  RemoteMR(Context &c);

  bool establish();
  void map (messages::Message*);
  void insert_idata(messages::Message*);
  void insert_igroup(messages::Message*);
  void insert_iblock(messages::Message*);
  void request_idata(messages::Message*);
  void request_igroup(messages::Message*);
  void request_iblock(messages::Message*);

 protected:
  PeerMR* peer_mr;
};

}  // namespace eclipse
