#include "ireader.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <boost/asio.hpp>
#include "../../common/context.hh"
#include "../../messages/factory.hh"
#include "../../messages/message.hh"
#include "../messages/igroupinforequest.hh"

using std::vector;
using std::ifstream;
using std::string;
using boost::asio::ip::tcp;

namespace eclipse {

IReader::IReader() {
  scratch_path_ = con.settings.get<string>("path.scratch");
  num_finished_ = 0;
  is_next_key_ = true;
  is_next_value_ = true;
  is_clear_ = true;
}
IReader::IReader(uint32_t net_id, uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id):
    IReader() {
  net_id_ = net_id;
  job_id_ = job_id;
  map_id_ = map_id;
  reducer_id_ = reducer_id;
  init();
}
IReader::~IReader() {
}
void IReader::init() {
  num_block_ = get_num_block();
  num_remain_.resize(num_block_);
  loaded_keys_.resize(num_block_);
  string block_path_prefix = scratch_path_ + "/.job_" + std::to_string(job_id_)
      + "_" + std::to_string(map_id_) + "_" + std::to_string(reducer_id_) + "_";
  for (uint32_t i = 0; i < num_block_; ++i) {
    string block_path = block_path_prefix + std::to_string(i);
    blocks_.push_back(new ifstream(block_path));
    LoadKey(i);
  }
  SetNext();
}
void IReader::set_net_id(uint32_t net_id) {
  net_id_ = net_id;
}
void IReader::set_job_id(uint32_t job_id) {
  job_id_ = job_id;
}
void IReader::set_map_id(uint32_t map_id) {
  map_id_ = map_id;
}
void IReader::set_reducer_id(uint32_t reducer_id) {
  reducer_id_ = reducer_id;
}
bool IReader::is_next_key() {
  return is_next_key_;
}
bool IReader::is_next_value() {
  return is_next_value_;
}
bool IReader::get_next_key(string &key) {
  if (!is_next_key_) return false;
  if (!is_clear_) {
    if (!ShiftToNextKey()) return false;
    is_clear_ = false;
  }
  SetNextAsCurrent();
  key = curr_key_;
  LoadValue(curr_block_index_);
  is_next_value_ = true;
  return true;
}
bool IReader::get_next_value(string &value) {
  value = next_value_;
  if (num_remain_[curr_block_index_] > 0) {
    LoadValue(curr_block_index_);
  } else {  // All the values of current key from current file have been read.
    if (!LoadKey(curr_block_index_)) {  // File ends.
      if (!FinishBlock(curr_block_index_)) return false;
    }
    SetNext();
    if (next_key_ == curr_key_) {  // Same key exist on another file.
      SetNextAsCurrent();
      LoadValue(curr_block_index_);
    } else {  // All the values of current key from whole files have been read.
      is_next_value_ = false;
      is_clear_ = true;
    }
  }
  return true;
}
tcp::socket* IReader::connect(uint32_t net_id) {
  tcp::socket *socket = new tcp::socket(io_service_);
  int port = con.settings.get<int>("network.port_mapreduce");
  vector<string> nodes = con.settings.get<vector<string>>("network.nodes");
  string host = nodes[net_id];
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(host, std::to_string(port));
  tcp::resolver::iterator it(resolver.resolve(query));
  auto ep = new tcp::endpoint(*it);
  socket->connect(*ep);
  delete ep;
  return socket;
}
void IReader::send_message(tcp::socket *socket, messages::Message *msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;
  socket->send(boost::asio::buffer(ss.str()));
}
messages::IGroupInfo* IReader::read_igroup_info(tcp::socket *socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char *body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  messages::Message *msg = messages::load_message(recv_msg);
  delete[] body;
  return dynamic_cast<messages::IGroupInfo*>(msg);
}
messages::IBlockInfo* IReader::read_iblock_info(tcp::socket *socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char *body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  messages::Message *msg = messages::load_message(recv_msg);
  delete[] body;
  return dynamic_cast<messages::IBlockInfo*>(msg);
}
uint32_t IReader::get_num_block() {
  // tcp::socket *socket = connect(net_id_);
  messages::IGroupInfoRequest ig_request;
  ig_request.job_id = job_id_;
  ig_request.map_id = map_id_;
  ig_request.reducer_id = reducer_id_;
  messages::IGroupInfo igroup_info;
  directory_.select_igroup_metadata(ig_request.job_id, ig_request.map_id,
      ig_request.reducer_id, &igroup_info);
  // send_message(socket, &ig_request);
  // auto igroup_info = read_igroup_info(socket);
  // socket->close();
  return igroup_info.num_block;
}
void IReader::SetNext() {
  next_it_ = get_min_iterator();
  next_block_index_ = next_it_->second;
  next_key_ = next_it_->first;
  is_next_key_ = true;
}
void IReader::SetNextAsCurrent() {
  key_order_.erase(next_it_);
  curr_block_index_ = next_block_index_;
  curr_key_ = next_key_;
}
bool IReader::ShiftToNextKey() {
  for (uint32_t i = 0; i < num_block_; ++i) {
    if (loaded_keys_[i] == curr_key_) {
      ifstream *block = blocks_[i];
      int pos = block->tellg();
      int num_remain = num_remain_[i];
      block->seekg(pos + num_remain);
      if (!LoadKey(i)) {  // File ends.
        if (!FinishBlock(i)) return false; // All the files end.
      }
    }
  }
  return true;
}
bool IReader::LoadKey(const int &index) {
  // Make sure you are not in the middle of the values.
  if (blocks_[index]->eof()) {
std::cout << "!!!! eof() works well!" << std::endl;
    return false;
  }
  getline(*blocks_[index], loaded_keys_[index]);
  string num_value;
  getline(*blocks_[index], num_value);
if (num_value == "") return false;
  num_remain_[index] = stoi(num_value);
  key_order_.emplace(loaded_keys_[index], index);
  return true;
}
bool IReader::LoadValue(const int &index) {
  getline(*blocks_[index], next_value_);
  --num_remain_[curr_block_index_];
  return true;
}
std::multimap<string, int>::iterator IReader::get_min_iterator() {
  return key_order_.begin();
}
bool IReader::FinishBlock(const int &index) {
  blocks_[index]->close();
  delete blocks_[index];
  loaded_keys_[index] = "";
  ++num_finished_;
  if (num_finished_ == num_block_) {
    is_next_value_ = false;
    is_next_key_ = false;
    return false;
  }
  return true;
}

}  // namespace eclipse
