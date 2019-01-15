#pragma once

#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <list>
#include <functional>
#include <vector>

#include "../mapreduce/output_collection.hh"

using before_map_t = void (*)(std::unordered_map<std::string, void*>&);
using after_map_t = void (*)(std::unordered_map<std::string, void*>&);
using mapper_t = void (*)(std::string&, velox::OutputCollection&, std::unordered_map<std::string, void*>&);
using mapper2_t = void (*)(std::string&, int, velox::OutputCollection&, velox::OutputCollection&, std::unordered_map<std::string, void*>&);
//
using mapper3_t = void (*)(std::vector<std::string>&, velox::OutputCollection&, std::unordered_map<std::string, int>&);
//
using reducer_t = void (*)(std::string&, std::vector<std::string>&);
//using reducer_t = void (*)(std::string&, std::vector<std::string>&, velox::OutputCollection&);

class DL_loader {
  public:
    DL_loader(std::string);
    ~DL_loader();

    bool init_lib ();
    before_map_t load_function_before_map (std::string);
    after_map_t load_function_after_map (std::string);
    mapper_t load_function (std::string);
    mapper2_t load_function_2 (std::string);
//
    mapper3_t load_function_3 (std::string);
//
    reducer_t load_function_reduce (std::string);
    void close();


  protected:
    std::string lib_name;
    void* lib= nullptr;
};
