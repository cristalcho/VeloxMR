// This is the message package of key values.
// Key values are packed some amount and will be sent to other nodes.
#ifndef ECLIPSEMR_MAPREDUCE_KV_BLOCK_H_
#define ECLIPSEMR_MAPREDUCE_KV_BLOCK_H_
#include <list>
#include <string>
#include <utility>
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct KvBlock: public Message{
  KvBlock() = default;
  ~KvBlock() = default;
  std::string get_type() const override;

  uint32_t job_id_;
  uint32_t map_id_;
  bool is_last_block_;
  std::list<std::pair<std::string, std::string>> kvs_;
};

}  // namespace messages
}  // namespace eclipse
#endif  // ECLIPSEMR_MAPREDUCE_KV_BLOCK_H_
