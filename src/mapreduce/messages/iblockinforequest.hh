#pragma once
#include <string>
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct IBlockInfoRequest: public Message {
  IBlockInfoRequest() = default;
  ~IBlockInfoRequest() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t reducer_id;
  uint32_t block_seq;
};

}
}
