#pragma once
#include "message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct NodesShuffling: public Message {
  std::string get_type() const override;

  std::vector<int> nodes;
  uint32_t job_id = 0;
};

}
}
