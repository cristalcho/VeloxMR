#ifndef ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#define ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_
#include <string>
#include <vector>
#include <map>
#include "messages/message.hh"
namespace eclipse {
namespace messages {

struct KeyValueShuffle: public Message {
  KeyValueShuffle() = default;
  ~KeyValueShuffle() = default;
  std::string get_type() const override;

  uint32_t job_id_ = 0;
  uint32_t map_id_ = 0;
  std::map<std::string, std::vector<std::string>> kv_pairs;
  uint32_t number_of_keys = 0; 
  uint32_t kv_id = 0;
  uint32_t node_id = 0;
  uint32_t origin_id = 0;
  
};

}  // namespace messages
}  // namespace eclipse
#endif  // ECLIPSEMR_MAPREDUCE_MESSAGES_KEYVALUESHUFFLE_H_

