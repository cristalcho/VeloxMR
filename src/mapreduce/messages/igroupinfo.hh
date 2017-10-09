#pragma once
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct IGroupInfo: public Message {
  IGroupInfo() = default;
  ~IGroupInfo() = default;

  std::string get_type() const override;

  uint32_t job_id = 0;
  uint32_t map_id = 0;
  uint32_t reducer_id = 0;
  uint32_t num_block = 0;
};

}
}

