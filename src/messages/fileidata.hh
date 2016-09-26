#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {
struct FileIData: public Message {
  FileIData() = default;
  ~FileIData() = default;
  std::string get_type() const override;

  std::string idata;
  uint32_t idata_hash_key = 0;
  uint64_t idata_size = 0;
  unsigned int num_block = 0;
  unsigned int replica = 0;
};
}
}
