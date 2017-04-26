#include "iwriter.h"
#include <string>
#include <list>
#include <map>
#include <memory>
#include <iterator>
#include <fstream>
#include <utility>
#include <thread>
#include <sstream>
#include <iomanip>
#include <exception>
#include <boost/asio.hpp>
#include "../../common/context_singleton.hh"
#include "../../messages/message.hh"
#include "../../messages/factory.hh"
#include "../../messages/reply.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"

using std::list;
using std::vector;
using std::multimap;
using std::string;
using boost::asio::ip::tcp;
using std::stringstream;
using std::setw;
using std::setfill;

// ###########################
// Rule for kmv_blocks_
// front     : Stacking block.
// front + n : Ready to write.
// ###########################
// Rule for naming iblocks
// .job_[job_id]_[map_id]_[reducer_id]_[block_seq]

namespace eclipse {

IWriter::IWriter() {
  reduce_slot_ = context.settings.get<int>("mapreduce.reduce_slot");
  iblock_size_ = context.settings.get<int>("mapreduce.iblock_size");
  scratch_path_ = context.settings.get<string>("path.idata");
  is_write_start_ = false;
  is_write_finish_ = false;
  index_counter_ = 0;
  writing_index_ = -1;
  write_buf_size_ = context.settings.get<int>("mapreduce.write_buf_size");
  written_bytes_ = 0;
  write_buf_ = (char*)malloc(write_buf_size_ + 1);
  write_pos_ = write_buf_;
  kmv_blocks_.resize(reduce_slot_);
  is_write_ready_.resize(reduce_slot_);
  mutex_end_thread.lock();
  mutex_start_thread.lock();
  for (uint32_t i = 0; i < reduce_slot_; ++i) {
    block_size_.emplace_back(0);
    write_count_.emplace_back(0);
  }
  writer_thread_ = std::make_unique<std::thread>(run, this);
}

IWriter::IWriter(const uint32_t job_id, const uint32_t map_id) : IWriter() {
  job_id_ = job_id;
  map_id_ = map_id;
}

void IWriter::set_job_id(const uint32_t job_id) {
  job_id_ = job_id;
}

void IWriter::set_map_id(const uint32_t map_id) {
  map_id_ = map_id;
}

bool IWriter::is_write_finish() {
  return is_write_finish_;
}

IWriter::~IWriter() {
  free(write_buf_);
}
void IWriter::finalize() {
  // It should be garuanteed no more key values added
  for (uint32_t i = 0; i < reduce_slot_; ++i) {
    if (kmv_blocks_[i].size() > 0) {
      is_write_ready_[i].front() = true;
    }
  }
  is_write_start_ = true;
  mutex_start_thread.unlock();
  mutex_end_thread.unlock();
  writer_thread_->join();

  for (uint32_t i = 0; i < reduce_slot_; ++i) {
    messages::IGroupInsert igroup_insert;
    igroup_insert.job_id = job_id_;
    igroup_insert.map_id = map_id_;
    igroup_insert.reducer_id = i;
    igroup_insert.num_block = write_count_[i];
    directory_.insert_igroup_metadata(igroup_insert);
  }
  messages::IDataInsert idata_insert;
  idata_insert.job_id = job_id_;
  idata_insert.map_id = map_id_;
  idata_insert.num_reducer = reduce_slot_;
  directory_.insert_idata_metadata(idata_insert);
}

void IWriter::run(IWriter *obj) {
  try {
    obj->seek_writable_block();
  } catch (std::exception& e) {
    ERROR("exeception in iwriter worker thread %s", e.what());
  }
}

void IWriter::seek_writable_block() {
  // while loops should be changed to lock
  mutex_start_thread.lock();
  while(!is_write_start_);
  while(!is_write_finish_) {
    // Check if there is any block that should be written to disk.
    // And if it's true, write it onto disk.
    mutex.lock();
    for (uint32_t i = 0; i < reduce_slot_; ++i) {
      if (kmv_blocks_[i].size() > 0 && is_write_ready_[i].back()) {
        auto writing_block = kmv_blocks_[i].back();
        kmv_blocks_[i].pop_back();
        is_write_ready_[i].pop_back();
        write_block(writing_block, i);
      }
    }

    // Check if there are no more incoming key value pairs.
    if(is_write_start_) {
      uint32_t finish_counter = 0;
      for (uint32_t i = 0; i < reduce_slot_; ++i) {
        if(kmv_blocks_[i].size() == 0) {
          ++finish_counter;
        }
      }
      if (finish_counter == reduce_slot_) {
        is_write_finish_ = true;
      }
    }
    mutex.unlock();
  }

  mutex_end_thread.lock();
}
void IWriter::add_key_value(const string &key, const string &value) {
  deb.insert(key);

  mutex.lock();
  int index;
  index = get_index(key);
  if (kmv_blocks_[index].size() == 0 || is_write_ready_[index].front()) {
    add_block(index);
  }
  auto block = kmv_blocks_[index].front();

  uint32_t new_size;
  if (block->find(key) == block->end()) {
    new_size = get_block_size(index) + key.length() + value.length() + 4;
  } else {
    new_size = get_block_size(index) + value.length() + 1;
  }
  block->insert({key, std::move(value)});
  set_block_size(index, new_size);

  if (new_size > iblock_size_) {
    is_write_ready_[index].front() = true;
    is_write_start_ = true;
    mutex_start_thread.unlock();
  }
  mutex.unlock();
}
void IWriter::add_block(const uint32_t index) {
  std::shared_ptr<std::multimap<string, string>> pt (new std::multimap<string, string>);
  kmv_blocks_[index].emplace_front(pt);
  is_write_ready_[index].push_front(false);
  block_size_[index] = 0;
}
int IWriter::get_block_size(const uint32_t index) {
  return block_size_[index];
}
void IWriter::set_block_size(const uint32_t index, const uint32_t size) {
  block_size_[index] = size;
}
int IWriter::get_index(const string &key) {
  auto it = key_index_.find(key);
  if (it != key_index_.end()) {
    return it->second;
  } else {
    int index = index_counter_ % reduce_slot_;
    ++index_counter_;
    key_index_.emplace(key, index);
    return index;
  }
}
void IWriter::write_record(const string &record) {
  int record_size = record.length();
  if (written_bytes_ + record_size > write_buf_size_)
    flush_buffer();
  buffer_record(record);
}
void IWriter::buffer_record(const string &record) {
  uint32_t record_size = record.length();
  strncpy(write_pos_, record.c_str(), record_size);
  write_pos_ += record_size;
  strncpy(write_pos_, "\n", 1);
  ++write_pos_;
  written_bytes_ += record_size + 1;
}
void IWriter::flush_buffer() {
  strncpy(write_pos_, "\0", 1);
  file_ << write_buf_;
  write_pos_ = write_buf_;
  written_bytes_ = 0;
}
void IWriter::set_writing_file(const uint32_t index) {
  writing_index_ = index;
  string file_path;
  get_new_path(file_path);
  file_.open(file_path);
}
void IWriter::write_block(std::shared_ptr<std::multimap<string, string>> block,
    const uint32_t index) {
  set_writing_file(index);
  std::pair<multimap<string, string>::iterator,
      multimap<string, string>::iterator> ret;
      int i = 0;


  for (auto key_it = block->begin(); key_it != block->end(); key_it =
      ret.second) {
    ret = block->equal_range(key_it->first);
    write_record(key_it->first);
    int num_value = std::distance(ret.first, ret.second);
    write_record(std::to_string(num_value));
    for (auto it = ret.first; it != ret.second; ++it) {
      write_record(it->second);
    }
    i++;
  } 
  block.reset();
  flush_buffer();
  file_.close();
  messages::IBlockInsert iblock_insert;
  iblock_insert.job_id = job_id_;
  iblock_insert.map_id = map_id_;
  iblock_insert.reducer_id = index;
  iblock_insert.block_seq = write_count_[index] - 1;
  directory_.insert_iblock_metadata(iblock_insert);
  block_size_[index] = 0;
}

void IWriter::get_new_path(string &path) {
  path = scratch_path_ + "/.job_" + std::to_string(job_id_) + "_" +
      std::to_string(map_id_) + "_" + std::to_string(writing_index_) + "_" +
      std::to_string(write_count_[writing_index_]);
  ++write_count_[writing_index_];
}

}  // namespace eclipse
