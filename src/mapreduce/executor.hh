#pragma once
#include "nodes/peermr.h"
#include "../messages/task.hh"

namespace eclipse {
class Executor {
  typedef void (*maptype)(std::string);
  public:
    Executor (PeerMR*);
    ~Executor ();

    bool run_map (messages::Task*, std::string);

  protected:
    PeerMR* peer;

};

} /* eclipse  */ 
