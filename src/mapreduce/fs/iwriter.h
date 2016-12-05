#ifndef ECLIPSEMR_FS_IWRITER_H_
#define ECLIPSEMR_FS_IWRITER_H_
#include <memory>
#include <thread>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <list>
#include <fstream>
#include <boost/asio.hpp>
#include "iwriter_interface.hh"
#include "../../common/context.hh"
#include "../../messages/message.hh"
#include "../../messages/reply.hh"
#include "../fs/directorymr.hh"

using std::list;
using std::vector;
using std::multimap;
using std::unordered_map;
using std::string;
using boost::asio::ip::tcp;

namespace eclipse {

class IWriter: public IWriter_interface {
 public:
  IWriter();
  IWriter(const uint32_t job_id, const uint32_t map_id);
  ~IWriter();

  void add_key_value(const string &key, const string &value) override;
  void set_job_id(const uint32_t job_id) override;
  void set_map_id(const uint32_t map_id) override;
  bool is_write_finish() override;
  void finalize() override;

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

  DirectoryMR directory_;
  std::unique_ptr<std::thread> writer_thread_;

  uint32_t job_id_;
  uint32_t map_id_;
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
  std::mutex mutex;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_FS_IWRITER_H_
