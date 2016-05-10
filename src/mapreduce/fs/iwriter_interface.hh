#pragma once 
#include <string>

namespace eclipse {

struct IWriter_interface {
  virtual void add_key_value(const std::string&, const std::string&) = 0;
  virtual void set_job_id(const uint32_t) = 0;
  virtual void set_map_id(const uint32_t) = 0;
  virtual bool is_write_finish() = 0;
  virtual void finalize() = 0;
};
}
