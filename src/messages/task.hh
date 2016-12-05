#pragma once

#include "message.hh"
#include <vector>
#include <utility>

namespace eclipse {
namespace messages {

struct Task: public Message {
  std::string get_type() const override;
  std::string type;
  std::string lang;
  std::string library, func_name, input_path;
  std::string file_output;
  std::string func_body;
  std::vector<std::pair<uint32_t, std::string>> blocks;
  uint32_t subjob_id = 0;
  uint32_t job_id = 0;
  uint32_t leader = 0;
};

}
}

