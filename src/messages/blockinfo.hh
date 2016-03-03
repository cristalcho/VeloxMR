#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockInfo: public Message {
    std::string get_type() const override;

    uint32_t file_id;
    unsigned int block_seq;
    uint32_t block_hash_key;
    std::string block_name;
    uint32_t block_size;
    unsigned int is_inter;
    std::string node;
    std::string l_node;
    std::string r_node;
    unsigned int is_commit;
    std::string content;
  };
}
}
