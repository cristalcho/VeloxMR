#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/hash.hh"
#include "../mapreduce/messages/key_value_shuffle.h"

#include <string>
#include <sstream>
#include <utility>
#include <functional>

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(PeerMR* p) : peer(p) { }
Executor::~Executor() { }
// }}}
// run_map {{{
bool Executor::run_map (messages::Task* m, std::string input) {
    auto path_lib = context.settings.get<string>("path.applications");
    path_lib += ("/" + m->library);
    DL_loader loader (path_lib);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      context.logger->error ("Not found library path[%s]", path_lib.c_str());
    }

    function<pair<string, string>(string)> _map_ = 
        loader.load_function(m->func_name);
    stringstream ss (input);

    while (!ss.eof()) {
      char next_line [256] = {0}; //! :TODO: change to DFS line limit
      ss.getline (next_line, 256);
      pair<string, string> key_value = _map_ (string(next_line));

      auto key        = key_value.first;
      auto hash_key   = h(key.c_str());
      auto& value     = key_value.second;

      KeyValueShuffle kv; 
      kv.job_id_ = 0;
      kv.key_ = key;
      kv.value_ = value; 
      peer->process(&kv);
      //peer->insert (hash_key, key, value);
    }

    return true;
  }
// }}}
} /* eclipse  */
