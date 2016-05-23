#pragma once

#include "message.hh"
//#include "mapreduce/task.hh"

namespace eclipse {
namespace messages {

struct Task: public Message {
  std::string get_type() const override;

  Task& set_id(int);
  Task& set_type(int);
  Task& set_input_path(std::string);

  int get_id();
  std::string get_type_task();
  std::string get_input_path();

  int id, map_id;
  uint32_t job_id;
  std::string type;
  std::string library, func_name, input_path;
  std::string output;
  std::string block_name;
  uint32_t block_hash_key;
};

}
}

