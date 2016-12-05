#pragma once

#include "message.hh"
#include <vector>
#include <utility>

namespace eclipse {
namespace messages {

struct Job: public Message {
  std::string get_type() const override;
  uint32_t job_id = 0;

  std::string type;
  std::string library;
  std::string map_name;
  std::string reduce_name;
  std::string file_output;
  std::vector<std::string> files;
  std::string func_body;
  std::string lang;
};

}
}

