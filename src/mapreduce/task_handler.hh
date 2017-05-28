#pragma once
#include "output_collection.hh"
#include <vector>
#include <string>
#include <unordered_map>

namespace eclipse {

typedef velox::OutputCollection TaskOutput; 
typedef std::vector<std::string> vec_str; 
typedef std::unordered_map<std::string, void*> TaskOptions;

class task_handler {
  public:

    task_handler();
    virtual ~task_handler() = default;

    virtual void setup(bool is_map) = 0;
    virtual void before_map(TaskOptions&) = 0;
    virtual void after_map(TaskOptions&) = 0;
    virtual void map(std::string&, TaskOutput&, TaskOptions&) = 0;
    virtual void reduce(std::string&, vec_str&, TaskOutput&) = 0;
};

}
