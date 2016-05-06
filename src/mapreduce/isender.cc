#include "isender.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <boost/asio.hpp>
#include "kv_block.h"
#include "../common/hash.hh"
#include "../messages/message.hh"
#include "../messages/factory.hh"

namespace eclipse {

ISender::ISender() {
}
ISender::~ISender() {
}
void ISender::add_key_value(const std::string &key, const std::string &value) {
  uint32_t kv_size = key.length() + value.length();
  if (current_size_ + kv_size > capacity_) {
    send_block(package_);
  }
  package_.kvs_.emplace_back(key, value);
  current_size_ += kv_size;
}
void ISender::send_block(const messages::KvBlock &kv_block) {
  
}
boost::asio::ip::tcp::socket* ISender::connect(const int hash_value) {
  boost::asio::ip::tcp::socket *socket =
      new boost::asio::ip::tcp::socket(iosvc_);
  int port = con_.settings.get<int>("network.port_mapreduce");
  std::vector<std::string> nodes =
      con_.settings.get<std::vector<std::string>>("network.nodes");
  string host = nodes[hash_value % node.size()];
  boost::asio::ip::tcp::resolver resolver(iosvc_);
  boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
  boost::asio::ip::tcp::resolver::iterator it(resolver.resolve(query));
  auto ep = new boost::asio::ip::tcp::endpoint(*it);
  socket->connect(*ep);
  delete ep;
  return socket;
}
void ISender::send_mesage(boost::asio::ip::tcp::socket *socket,
    messages::Message *msg) {
  std::string out = save_message(msg);
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(16) << out.length() << out;
  socket->send(boost::asio::buffer(ss.str()));
}
