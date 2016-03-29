#ifndef ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#define ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#include <string>
#include "../../messages/message.hh"
namespace eclipse {
namespace messages {

struct KeyValueShuffle: public Message {
  KeyValueShuffle() = default;
  ~KeyValueShuffle() = default;
  std::string get_type() const override;

  uint32_t job_id_;
  uint32_t map_id_;
  // bool is_last_kv_;
  std::string key_;
  std::string value_;
};

}  // namespace messages
}  // namespace eclipse
#endif  // ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_

