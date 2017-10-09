#pragma once
#include "../../messages/message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct NodesShuffling: public Message {
  std::string get_type() const override;

  std::vector<int> nodes;
  uint32_t job_id = 0;
  uint32_t id = 0;
  uint32_t kv_id = 0;
};

}
}
