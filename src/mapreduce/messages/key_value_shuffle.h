#ifndef ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#define ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#include <string>
#include <vector>
#include "../../messages/message.hh"
namespace eclipse {
namespace messages {

struct KeyValueShuffle: public Message {
  KeyValueShuffle() = default;
  ~KeyValueShuffle() = default;
  std::string get_type() const override;

  uint32_t job_id_;
  uint32_t map_id_;
  std::string key_;
  std::vector<std::string> value_;
  bool is_header = false;
  uint32_t number_of_keys = 0; 
  
};

}  // namespace messages
}  // namespace eclipse
#endif  // ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_

