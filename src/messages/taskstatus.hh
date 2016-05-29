#pragma once
#include "message.hh"

namespace eclipse {
namespace messages {

struct TaskStatus: public Message {
  std::string get_type() const override;

  uint32_t job_id = 0;
  bool is_success = false;
};
  
} /* messages  */ 
}
