#include "client_handler.hh"
#include "../messages/factory.hh"
#include "../common/context_singleton.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/spawn.hpp>
#include <mutex>

using namespace eclipse::network;
using namespace std;

mutex mut;
// Constructor {{{
ClientHandler::ClientHandler (uint32_t p):
  nodes(context.settings.get<vec_str> ("network.nodes")),
  port(p)
{ 
}
// }}}
// connect {{{
void ClientHandler::connect(uint32_t i, shared_ptr<Server> server) {
  spawn(context.io, [&, server_copy=server, node=nodes[i], p=this->port](boost::asio::yield_context yield) {
      try {
      shared_ptr<Server> s = server_copy;
      boost::system::error_code ec;
      tcp::resolver resolver (context.io);
      tcp::resolver::query query (node, to_string(p));

      auto it = resolver.async_resolve(query, yield[ec]);
      if (ec) {
      ERROR("Resolving %s:%u", node.c_str(), p);
      return;
      }

      tcp::endpoint ep (*it);
      while (true) {
        s->get_socket().async_connect(ep, yield[ec]);
        if (ec) {
          if(ec == boost::asio::error::timed_out) {
            s->get_socket().close();
            WARN("Re-connecting to %s:%u", node.c_str(), p);
            continue;
          }
          ERROR("Connecting %s:%u ec=%s", node.c_str(), p, ec.message().c_str());
          return;
        }
        break;
      }

      boost::asio::ip::tcp::no_delay option(true);
      s->get_socket().set_option(option); 

//      current_servers.insert({i, s});
      s->do_write_buffer();

      } catch (exception& e) {
        INFO("Connect handler exception %s", e.what());
      } catch (boost::exception& e) {
        INFO("Connect handler exception %s", diagnostic_information(e).c_str());
      }
      });
}
// }}}
// send {{{
bool ClientHandler::send(uint32_t i, messages::Message* m) {
  if (i >= nodes.size()) return false;
 // mut.lock();

  // If connection is still on.
  //if (current_servers.find(i) != current_servers.end()) {
  //  shared_ptr<Server> ptr = current_servers[i].lock();
  //  if (ptr) {
  //    ptr->do_write(m);
  //    return true;
  //  }
  //} 

  auto server = make_shared<Server>(node);
  shared_ptr<std::string> message_serialized (save_message(m));
  server->commit(message_serialized);
  connect(i, server);

  //mut.unlock();

  return true;
}
// }}}
// send str{{{
bool ClientHandler::send(uint32_t i, shared_ptr<string> str) {
  if (i >= nodes.size()) return false;

  // If connection is still on.
  if (current_servers.find(i) != current_servers.end()) {
    shared_ptr<Server> ptr = current_servers[i].lock();
    if (ptr) {
      ptr->do_write(str);
      return true;
    }
  } 

  auto server = make_shared<Server>(node);
  server->commit(str);
  connect(i, server);

  return true;
}
// }}}
// send_and_replicate {{{
bool ClientHandler::send_and_replicate(std::vector<int> node_indices, messages::Message* m) {
  shared_ptr<std::string> message_serialized (save_message(m));
  for (auto i : node_indices) {
    send(i, message_serialized);
  }
  return true;
}
// }}}
