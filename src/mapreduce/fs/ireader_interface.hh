#pragma once 
#include <string>

namespace eclipse {

struct IReader_interface {
  virtual void init() = 0;
  virtual void set_net_id(uint32_t) = 0;
  virtual void set_job_id(uint32_t) = 0;
  virtual void set_map_id(uint32_t) = 0;
  virtual void set_reducer_id(uint32_t) = 0;
  virtual bool get_next_key(std::string&) = 0;
  virtual bool get_next_value(std::string&) = 0;
  virtual bool is_next_key() = 0;
  virtual bool is_next_value() = 0;
};
}
