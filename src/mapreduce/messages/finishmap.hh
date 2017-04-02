#pragma once
#include <string>
#include <vector>
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct FinishMap: public Message {
  FinishMap() = default;
  ~FinishMap() = default;

  std::string get_type() const override;

  uint32_t job_id = 0;
  std::vector<uint32_t> nodes;
};

}
}
