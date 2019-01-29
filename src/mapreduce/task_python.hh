#pragma once
#include "task_handler.hh"
#include "dl_loader.hh"
#include <memory>

extern "C" typedef struct _object PyObject;

namespace eclipse {

class task_python : public task_handler {
  public:
    task_python(std::string function_body, std::string before_map, std::string after_map);
    ~task_python();

    void setup(bool is_map) override;
    void before_map(TaskOptions&) override;
    void after_map(TaskOptions&) override;
    void map(std::string&, TaskOutput&, TaskOptions&) override;
    void reduce(std::string&, vec_str&, TaskOutput&) override;

  private:
    std::string function_body_f;
    std::string before_map_f;
    std::string after_map_f;

    PyObject* python_module;
};

}
