#include "remotemr.h"
#include <functional>
#include "peermr.h"
#include "../../messages/reply.hh"
#include "../messages/idatalist.hh"

using namespace eclipse::messages;
namespace ph = std::placeholders;
using std::bind;

using namespace eclipse;

// Constructor {{{
RemoteMR::RemoteMR(PeerMR* p, network::Network* net) : RemoteDFS(p, net) {
  peer = dynamic_cast<PeerMR*>(peer_dfs);
  auto& rt = routing_table;
  rt.insert({"Job", bind(&RemoteMR::job_handler, this, ph::_1, ph::_2)});
  rt.insert({"IDataList", bind(&RemoteMR::list_idata, this, ph::_1, ph::_2)});
}
//}}}
// job_handler {{{
void RemoteMR::job_handler (messages::Message* _m, int n) {
  auto m = dynamic_cast<Job*>(_m);
  logger->info("Job received.");

  peer->process_job(m, std::bind([](decltype(network) net, int n) { 
          Reply reply;
          reply.message = "OK";
          net->send(n, &reply);
        }, 
        network, n));
}
// }}}
// list_idata {{{
void RemoteMR::list_idata(messages::Message *msg, int n) {
  auto reply = peer->request_idata_list();
  network->send(n, &reply);
}
// }}}
