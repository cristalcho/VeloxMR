#pragma once
#include <string>
#include "messages/message.hh"

namespace eclipse {
namespace messages {

struct IBlockInsert: public Message {
  IBlockInsert() = default;
  ~IBlockInsert() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t reducer_id;
  uint32_t block_seq;
};

}
}
