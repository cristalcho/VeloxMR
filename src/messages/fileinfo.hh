#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Message {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;

  std::string name;
  uint32_t hash_key = 0;
  uint64_t size = 0;
  unsigned int num_block = 0;
  unsigned int type;
  unsigned int replica = 0;
};

}
}

