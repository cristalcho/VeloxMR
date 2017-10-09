#pragma once
#include "../../messages/message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct TaskStatus: public Message {
  std::string get_type() const override;

  uint32_t job_id = 0;
  uint32_t saved_keys = 0;
  bool is_success = false;
  std::vector<uint32_t> shuffled_nodes;
  std::string type;
};
  
} /* messages  */ 
}
