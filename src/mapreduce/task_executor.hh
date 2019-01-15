#pragma once
#include "../nodes/node.hh"
#include "../blocknode/block_node.hh"
#include "../messages/message.hh"

#include "fs/directorymr.hh"
#include "fs/iwriter_interface.hh"
#include "client/dfs.hh"
#include "messages/job.hh"
#include "messages/job.hh"
#include "messages/task.hh"
#include "messages/taskstatus.hh"
#include "messages/finishmap.hh"
#include "messages/nodes_shuffling.hh"
#include "messages/key_value_shuffle.h"

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <set>

namespace eclipse {

class TaskExecutor : public Node {
  public:
    TaskExecutor(network::ClientHandler*);
    ~TaskExecutor();
   // ~TaskExecutor() = default;

    void job_accept(messages::Job* job, std::function<void(void)> fn);
    void task_accept(messages::Task* task);
    void task_accept_status(messages::TaskStatus* status);
    void key_value_store(KeyValueShuffle* kv);
    void insert_key_value(messages::KeyValueShuffle *kv);
    void shuffle_is_done(uint32_t, uint32_t);

    void map_finish_notify(messages::FinishMap*);
    void shuffle_finish_notify(uint32_t job_id);
    void notify_map_is_finished(uint32_t job_id, std::vector<uint32_t>);
    void notify_task_leader(int, uint32_t, std::string);

    void notice_the_end();

  protected:
    void schedule_reduce(messages::Job*);
    void request_local_map(messages::Task*);
    void request_local_reduce(messages::Task*);

    void write_key_value(messages::KeyValueShuffle *key_value);
    void try_finish_map(uint32_t job_id);

//soojeong
    std::unordered_multimap<std::string, void*>** uma;
    std::vector<std::string> memoryPoints;
    std::vector<std::string>* mp;
//

    std::unordered_map<uint32_t, uint32_t> tasks_remaining;
    std::unordered_map<uint32_t, std::function<void(void)>> jobs_callback;
    std::unordered_map<uint32_t, std::shared_ptr<IWriter_interface>> iwriters_;
    std::set<uint32_t> nodes_shuffling;

    std::map<uint32_t, FinishMap> tasker_remaining_job;
    std::multiset<uint32_t> tasker_remaining_nodes_shuffling;

    std::mutex local_mut;
    std::mutex local_mut2;

    uint32_t current_nodes_shuffling = 0;
    uint32_t network_size;
    DirectoryMR directory;
    velox::DFS dfs;
    Local_io local_io;
};

}
