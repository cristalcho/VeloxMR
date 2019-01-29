#pragma once
#include "messages/message.hh"

namespace eclipse {
namespace messages {

struct IGroupInfoRequest: public Message {
  IGroupInfoRequest() = default;
  ~IGroupInfoRequest() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t reducer_id;
};

}
}
