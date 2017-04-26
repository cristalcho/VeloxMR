#include "dl_loader.hh"
#include <dlfcn.h>
#include <stdexcept>

using namespace std;

// Constructor& destructors {{{
DL_loader::DL_loader(string lib):
 lib_name(lib){ }
DL_loader::~DL_loader() {
  if(!lib)
    close();
}
// }}}
// init_executor {{{
bool DL_loader::init_lib () {
  lib = dlopen(lib_name.c_str(), RTLD_LAZY);

  if (!lib) throw std::runtime_error("Path not found" + string(dlerror()));

  return true;
}
// }}}
// load_function_before_map {{{
before_map_t DL_loader::load_function_before_map (std::string fun) {
  before_map_t func_ = 
    reinterpret_cast<before_map_t>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) return nullptr; //throw std::runtime_error("DL_LOADER: Symbol not found");
  return func_;
}
// }}}
// load_function_after_map {{{
after_map_t DL_loader::load_function_after_map (std::string fun) {
  after_map_t func_ = 
    reinterpret_cast<after_map_t>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) return nullptr; //throw std::runtime_error("DL_LOADER: Symbol not found");
  return func_;
}
// }}}
// load_function {{{
mapper_t DL_loader::load_function (std::string fun) {
  mapper_t func_ = 
    reinterpret_cast<mapper_t>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) throw std::runtime_error("DL_LOADER: Symbol not found");
  return func_;
}
// }}}
// load_function {{{
reducer_t DL_loader::load_function_reduce (std::string fun) {
  reducer_t func_ = 
    reinterpret_cast<reducer_t>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) throw std::runtime_error("Symbol not found");
  return func_;
}
// }}}
void DL_loader::close() {
  dlclose(lib);
}
