#pragma once
#include "task_handler.hh"
#include "dl_loader.hh"
#include <memory>

namespace eclipse {

class task_cxx : public task_handler {
  public:
    task_cxx(std::string library, std::string func_name);
    ~task_cxx();

    void setup(bool is_map) override;
    void before_map(TaskOptions&) override;
    void after_map(TaskOptions&) override;
    void map(std::string&, TaskOutput&, TaskOptions&); 
    void map(std::string&, int, TaskOutput&, TaskOutput&, TaskOptions&);
    void map(vec_str&, TaskOutput&, centOptions&);
    void reduce(std::string&, vec_str&) override;

  private:
    before_map_t before_map_f;
    after_map_t after_map_f;
    mapper_t mapper;
    mapper2_t mapper2nd;
    mapper3_t mapper3rd;
    reducer_t reducer;

    std::string library_path;
    std::string func_name;

    std::unique_ptr<DL_loader> loader;
};

}
