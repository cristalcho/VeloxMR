#include <mapreduce/nodes/remotemr.h>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  int in_port = context.settings.get<int>("network.ports.internal");
  int ex_port = context.settings.get<int>("network.ports.client");

  auto internal_net = make_unique<network::AsyncNetwork<P2P>> (in_port);
  PeerMR peer (internal_net.get());
  internal_net->establish();

  auto external_net = make_unique<network::AsyncNetwork<Server>> (ex_port);
  RemoteMR remote (&peer, external_net.get());
  external_net->establish();

  context.join();

  return EXIT_SUCCESS;
}
