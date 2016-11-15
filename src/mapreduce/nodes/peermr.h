#ifndef ECLIPSEMR_NODES_PEERMR_H_
#define ECLIPSEMR_NODES_PEERMR_H_
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "../../nodes/peerdfs.hh"
#include "../fs/directorymr.hh"
#include "../fs/iwriter_interface.hh"
#include "../../messages/message.hh"
#include "../messages/idatalist.hh"
#include "../messages/key_value_shuffle.h"
#include "../messages/finish_shuffle.h"
#include "../messages/job.hh"
#include "../messages/subjob.hh"
#include "../messages/task.hh"

namespace eclipse {

class PeerMR: public PeerDFS {
 public:
  PeerMR(network::Network*);
  ~PeerMR() = default;

  void on_read(messages::Message *msg, int) override;
  bool format();
  IDataList request_idata_list();

  bool process_job(messages::Job*, std::function<void(void)>);
  void submit_block(messages::BlockInfo*);
  template<typename T> void process(T);

 protected:
  bool is_leader(std::string);
  void notify_task_leader(int, uint32_t, uint32_t, std::string);

  void schedule_map(messages::SubJob*);
  void schedule_reduce(messages::Job*);

  void request_local_map(messages::Task*);
  void request_local_reduce(messages::Task*);
  void request_save_idata(int);

  void run_map_onto_block(std::string, std::string, messages::Task*);
  void write_key_value(messages::KeyValueShuffle *key_value);

  std::unordered_map<uint32_t, uint32_t> subjobs_remaining;
  std::unordered_map<uint32_t, uint32_t> tasks_remaining;
  std::unordered_map<uint32_t, std::function<void(void)>> jobs_callback;
  std::unordered_map<uint32_t, std::shared_ptr<IWriter_interface>> iwriters_;
  std::unordered_map<uint32_t, std::set<int>> shuffled_nodes;
  std::unordered_map<uint32_t, std::set<std::string>> shuffled_keys;
  std::unordered_map<uint32_t, std::vector<std::string>> stored_idata;
  DirectoryMR directory;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_NODES_PEERMR_H_
