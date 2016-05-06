#pragma once
#include "../../messages/message.hh"

namespace eclipse {
namespace messages {

struct IDataInfoRequest: public Message {
  IDataInfoRequest() = default;
  ~IDataInfoRequest() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
};

}  // namespace messages
}  
