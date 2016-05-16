#pragma once
#include "nodes/peermr.h"
#include "../messages/task.hh"
#include <utility>

namespace eclipse {
class Executor {
  typedef std::pair<string, string> (*maptype)(std::string);
  typedef std::string (*reducetype)(std::string, std::string);
  public:
    Executor (PeerMR*);
    ~Executor ();

    bool run_map (messages::Task*, std::string);
    bool run_reduce (messages::Task*);

  protected:
    PeerMR* peer;

};

} /* eclipse  */ 
