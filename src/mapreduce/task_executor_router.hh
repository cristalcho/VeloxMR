#pragma once
#include "../network/router_decorator.hh"
#include "task_executor.hh"

namespace eclipse {

//!
class TaskExecutorRouter: public RouterDecorator {
  public:
    TaskExecutorRouter(TaskExecutor*, Router*);
    ~TaskExecutorRouter() {};

  protected:
    TaskExecutor* executor;

    void job_accept(messages::Message*, Channel*);
    void task_accept(messages::Message*, Channel*);
    void task_status_accept(messages::Message*, Channel*);
    void keyvalue_accept(messages::Message*, Channel*);
    void finish_map(messages::Message*, Channel*);
    void finish_shuffle(messages::Message*, Channel*);
    void nodes_shuffling(messages::Message*, Channel*);
};

} /* eclipse  */ 
