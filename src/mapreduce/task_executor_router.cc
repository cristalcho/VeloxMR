#include "task_executor_router.hh"
#include "../common/context_singleton.hh"
#include "../messages/boost_impl.hh"
#include "messages/finish_shuffle.h"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
namespace ph = std::placeholders;

// Constructor {{{
TaskExecutorRouter::TaskExecutorRouter (TaskExecutor* exec, Router* router) : RouterDecorator(router) {
  executor = exec;

  using namespace std::placeholders;
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto& rt = routing_table;
  rt.insert({"Job", bind(&TaskExecutorRouter::job_accept, this, _1, _2)});
  rt.insert({"Task", bind(&TaskExecutorRouter::task_accept, this, _1, _2)});
  rt.insert({"TaskStatus", bind(&TaskExecutorRouter::task_status_accept, this, _1, _2)});
  rt.insert({"KeyValueShuffle", bind(&TaskExecutorRouter::keyvalue_accept, this, _1, _2)});
  rt.insert({"FinishMap", bind(&TaskExecutorRouter::finish_map, this, _1, _2)});
  rt.insert({"FinishShuffle", bind(&TaskExecutorRouter::finish_shuffle, this, _1, _2)});
  rt.insert({"NodesShuffling", bind(&TaskExecutorRouter::nodes_shuffling, this, _1, _2)});
}
// }}}
// job_accept {{{
void TaskExecutorRouter::job_accept(Message* m, Channel* channel) {
  Job* job = dynamic_cast<Job*>(m);
  executor->job_accept(job, std::bind([](Channel* channel) { 
          Reply reply;
          reply.message = "OK";
          channel->do_write(&reply);
        }, 
        channel));
}
// }}}
// task_accept {{{
void TaskExecutorRouter::task_accept(messages::Message* m , Channel*) {
  executor->task_accept(dynamic_cast<Task*>(m));
}
// }}}
// task_status_accept {{{
void TaskExecutorRouter::task_status_accept(messages::Message* m, Channel*) {
  executor->task_accept_status(dynamic_cast<TaskStatus*>(m));
}
// }}}
// keyvalue_accept {{{
void TaskExecutorRouter::keyvalue_accept(messages::Message* m, Channel*) {
  executor->key_value_store(dynamic_cast<KeyValueShuffle*>(m));
}
// }}}
// finish_map {{{
void TaskExecutorRouter::finish_map(messages::Message* m_, Channel*) {
  FinishMap* fm = dynamic_cast<FinishMap*> (m_);
  executor->map_finish_notify(fm);
}
// }}}
// finish_shuffle {{{
void TaskExecutorRouter::finish_shuffle(messages::Message* m, Channel*) {
  FinishShuffle* fs = dynamic_cast<FinishShuffle*> (m);
  executor->shuffle_finish_notify(fs->job_id_);
}
// }}}
// nodes_shuffling {{{
void TaskExecutorRouter::nodes_shuffling(messages::Message* m, Channel*) {
  NodesShuffling* ns = dynamic_cast<NodesShuffling*> (m);
  executor->shuffle_is_done(ns->job_id, ns->id);
}
// }}}
