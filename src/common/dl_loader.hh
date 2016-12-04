#pragma once

#include <string>
#include <utility>
#include <map>
#include <functional>

#include "../mapreduce/map_output_collection.hh"

//typedef std::pair<std::string, std::string>(*maptype)(std::string);
typedef std::string(*reducetype)(std::string, std::string);
using mapper_t = void (*)(std::string, velox::MapOutputCollection&);

class DL_loader {
  public:
    DL_loader(std::string);
    ~DL_loader();

    bool init_lib ();
    //maptype load_function (std::string);
    mapper_t load_function (std::string);
    reducetype load_function_reduce (std::string);
    void close();


  protected:
    std::string lib_name;
    void* lib= nullptr;
};
