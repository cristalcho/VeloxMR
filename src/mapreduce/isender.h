#ifndef ECLIPSEMR_MAPREDCUE_ISENDER_H_
#define ECLIPSEMR_MAPREDCUE_ISENDER_H_
#include <string>
#include <boost/asio.hpp>
#include "../mapreduce/kv_block.h"
#include "../common/context.hh"
#include "../messages/message.hh"

namespace eclipse {

class ISender {
 public:
  ISender();
  ~ISender();
  add_key_value(const std::string &key, const std::string & value);

 private:
  void send_block(const messages::KvBlock &kv_block);
  boost::asio::ip::tcp::socket* connect(const int hash_value);
  void send_message(boost::asio::ip::tcp::socket *socket,
      messages::Message *msg);

  uint32_t current_size_;
  uint32_t capacity_;
  boost::asio::io_service iosvc_;
  messages::KvBlock package_;
  Context con_;
};

}
#endif  // ECLIPSEMR_MAPREDUCE_ISENDER_H_

