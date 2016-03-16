#ifndef SRC_FS_IWRITER_H_
#define SRC_FS_IWRITER_H_
#include <memory>
#include <thread>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <fstream>
#include <boost/asio.hpp>
#include "../common/context.hh"

using std::list;
using std::vector;
using std::multimap;
using std::unordered_map;
using std::string;

namespace eclipse {

class IWriter {
 public:
  IWriter();
  IWriter(const uint32_t job_id, const uint32_t net_id);
  ~IWriter();

  void add_key_value(const string &key, const string &value);
  void set_job_id(const uint32_t job_id);
  void set_net_id(const uint32_t net_id);
  bool is_write_finish();
  void finalize();

 private:
  static void run(IWriter *obj);
  void seek_writable_block();
  void async_flush(const uint32_t index);
  void flush_buffer();
  void add_block(const uint32_t index);
  int get_block_size(const uint32_t index);
  void set_block_size(const uint32_t index, const uint32_t size);
  int get_index(const string &key);
  void write_record(const string &record);
  void buffer_record(const string &record);
  void set_file(const uint32_t index);
  void write_block(std::shared_ptr<multimap<string, string>> block,
      const uint32_t index);
  void set_writing_file(const uint32_t index);
  void get_new_path(string &path);
  //void flush(const uint32_t index);
  // void write_handler(const boost::system::error_code &ec,
  //     std::size_t bytes_transferred);

  std::unique_ptr<std::thread> writer_thread;
  Context con;
  uint32_t job_id_;
  uint32_t net_id_;
  uint32_t reduce_slot_;
  uint32_t iblock_size_;
  string scratch_path_;
  bool is_write_start_;
  bool is_write_finish_;
  uint32_t index_counter_;
  uint32_t writing_index_;
  uint32_t write_buf_size_;
  uint32_t written_bytes_;
  char *write_buf_;
  char *write_pos_;
  vector<list<std::shared_ptr<multimap<string, string>>>> kmv_blocks_;
  vector<list<bool>> is_write_ready_;
  vector<int> block_size_;
  vector<int> write_count_;
  unordered_map<string, int> key_index_;  // index of key
  std::ofstream file_;
  //boost::asio::io_service io_service_;
};

}  // namespace eclipse
#endif  // SRC_FS_IWRITER_H_
