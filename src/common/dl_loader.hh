#pragma once

#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <list>
#include <functional>

#include "../mapreduce/map_output_collection.hh"

using before_map_t = void (*)(std::unordered_map<std::string, void*>&);
using after_map_t = void (*)(std::unordered_map<std::string, void*>&);
using mapper_t = void (*)(std::string&, velox::MapOutputCollection&, std::unordered_map<std::string, void*>&);
using reducer_t = void (*)(std::string&, std::list<std::string>&, velox::MapOutputCollection&);

class DL_loader {
  public:
    DL_loader(std::string);
    ~DL_loader();

    bool init_lib ();
    before_map_t load_function_before_map (std::string);
    after_map_t load_function_after_map (std::string);
    mapper_t load_function (std::string);
    reducer_t load_function_reduce (std::string);
    void close();


  protected:
    std::string lib_name;
    void* lib= nullptr;
};
