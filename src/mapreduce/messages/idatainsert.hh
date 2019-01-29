#ifndef ECLIPSEMR_MESSAGES_IDATAINSERT_HH_
#define ECLIPSEMR_MESSAGES_IDATAINSERT_HH_
#include "messages/message.hh"

namespace eclipse {
namespace messages {

struct IDataInsert: public Message {
  IDataInsert() = default;
  ~IDataInsert() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t num_reducer;
};

}  // namespace messages
}  // namespace eclipse
#endif  // ECLIPSEMR_MESSAGES_IDATAINSERT_HH_
