#pragma once
#include <string>
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct FinishShuffle: public Message {
  FinishShuffle() = default;
  ~FinishShuffle() = default;

  std::string get_type() const override;

  uint32_t job_id_ = 0;
  uint32_t map_id_ = 0;
  uint32_t node_id = 0;
  uint32_t kv_id = 0;
};

}
}
