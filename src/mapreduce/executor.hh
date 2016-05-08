#pragma once
#include "nodes/peermr.h"
#include "../messages/task.hh"
#include <utility>

namespace eclipse {
class Executor {
  typedef std::pair<string, string> (*maptype)(std::string);
  public:
    Executor (PeerMR*);
    ~Executor ();

    bool run_map (messages::Task*, std::string);

  protected:
    PeerMR* peer;

};

} /* eclipse  */ 
