#pragma once

#include "../../messages/message.hh"
#include <vector>
#include <utility>

namespace eclipse {
namespace messages {

struct Task: public Message {
  Task()= default;
  ~Task() = default;
  virtual std::string get_type() const override;
  std::string type;
  std::string lang;
  std::string library, func_name, input_path;
  std::string file_output;
  std::string func_body, before_map, after_map;
  std::vector<std::pair<uint32_t, std::string>> blocks;
  uint32_t subjob_id = 0;
  uint32_t job_id = 0;
  uint32_t leader = 0;
  uint32_t id = 0;
};

}
}

