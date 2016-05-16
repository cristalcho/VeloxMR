#pragma once

#include <string>
#include <utility>

typedef std::pair<std::string, std::string>(*maptype)(std::string);
typedef std::string(*reducetype)(std::string, std::string);
class DL_loader {
  public:
    DL_loader(std::string);
    ~DL_loader();

    bool init_lib ();
    maptype load_function (std::string);
    reducetype load_function_reduce (std::string);
    void close();


  protected:
    std::string lib_name;
    void* lib= nullptr;
};
