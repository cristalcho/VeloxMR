#pragma once
#include "message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct IDataKeys: public Message {
  std::string get_type() const override;

  std::vector<std::string> keys;
  uint32_t job_id = 0;
};

}
}
