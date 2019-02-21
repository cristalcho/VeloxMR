#pragma once
#include "task_executor.hh"
#include "messages/task.hh"
#include <utility>

namespace eclipse {
class Executor {
  //typedef std::pair<string, string> (*maptype)(std::string);
  //typedef std::string (*reducetype)(std::string, std::string);
  public:
    Executor (TaskExecutor*, std::unordered_multimap<std::string, void*>**, std::vector<std::string>*);
    ~Executor ();

    bool run_map (messages::Task*);
    bool run_reduce (messages::Task*);

  protected:
    TaskExecutor* peer;
    std::unordered_multimap<std::string, void*>** umaparray;
    std::vector<std::string>* memoryPoints;
};

} /* eclipse  */ 
