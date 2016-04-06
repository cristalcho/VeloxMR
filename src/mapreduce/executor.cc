#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../common/hash.hh"

#include <string>
#include <sstream>

using namespace eclipse;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(PeerMR* p) : peer(p) { }
Executor::~Executor() { }
// }}}
// run_map {{{
bool Executor::run_map (messages::Task* m, std::string input) {
    DL_loader loader (m->library);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      //logger->error ("Not found library path[%s]", m->library.c_str());
    }

    auto _map_ = loader.load_function(m->func_name);
    stringstream ss (input);

    while (ss.eof()) {
      char next_line [256];
      ss.getline (next_line, 256);
      auto key_value = _map_ (string(next_line));

      auto key        = key_value.first;
      auto hash_key   = h(key.c_str());
      auto& value     = key_value.second;
      peer->insert (hash_key, key, value);
    }

    return true;
  }
// }}}
} /* eclipse  */
