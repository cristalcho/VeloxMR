#pragma once
#include "../../nodes/remotedfs.hh"
#include "../../messages/message.hh"
#include "peermr.h"

namespace eclipse {

class RemoteMR: public RemoteDFS {
 public: 
   RemoteMR (PeerMR*, network::Network*);
   void job_handler(messages::Message*, int);
   void list_idata(messages::Message*, int);

 protected:
  PeerMR* peer;
};

}  // namespace eclipse
