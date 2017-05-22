#include "task_cxx.hh"
#include "../common/context_singleton.hh"

#include <exception>
#include <memory>
#include <utility>

using namespace eclipse;
using namespace std;

// Constructor {{{
task_cxx::task_cxx(std::string library, std::string func_name) :
  library_path(library),
  func_name(func_name)
{
  std::string path_lib;
  try {
    path_lib = GET_STR("path.applications");
    path_lib += ("/" + library);
    loader.reset(new DL_loader(path_lib));

    loader->init_lib();

  } catch (std::exception& e) {
    ERROR("Not found library path[%s]", path_lib.c_str());
  }
}

task_cxx::~task_cxx() {

}
// }}}
// setup {{{
void task_cxx::setup(bool is_map) {

  if (is_map) {
    before_map_f = loader->load_function_before_map("before_map");
    after_map_f = loader->load_function_after_map("after_map");
    mapper = loader->load_function(func_name);

  } else {
    reducer = loader->load_function_reduce(func_name);
  }
}
// }}}
// pre_map {{{
void task_cxx::pre_map(TaskOptions& options) {
  if(before_map_f != nullptr)
    before_map_f(options);
}
//}}}
// after_map {{{
void task_cxx::after_map(TaskOptions& options) {
  if(after_map_f != nullptr)
    after_map_f(options);
}
// }}}
// map {{{
void task_cxx::map(std::string& line, TaskOutput& out, TaskOptions& options) {
  mapper(line, out, options);
}
// }}}
// reduce {{{
void task_cxx::reduce(std::string& key, vec_str& values, TaskOutput& out) {
  reducer(key, values, out);
}
// }}}
