#pragma once

#include "message.hh"
#include <vector>
#include <utility>
//#include "mapreduce/task.hh"

namespace eclipse {
namespace messages {

struct Task: public Message {
  std::string get_type() const override;

  Task& set_type(int);
  Task& set_input_path(std::string);

  std::string get_type_task();
  std::string get_input_path();

  int map_id = 0;
  uint32_t job_id = 0;
  std::string type;
  std::string library, func_name, input_path;
  std::vector<std::pair<uint32_t, std::string>> blocks;
};

}
}

