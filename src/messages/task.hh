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

  int id, job_id, map_id;
  std::string type;
  std::string library, func_name, input_path;
  std::string output;
};

//eclipse::messages::Task* serialize (eclipse::Task*);
//eclipse::Task* deserialize (Task*);

}
}

