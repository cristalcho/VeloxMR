#ifndef ECLIPSEMR_FS_IREADER_H_
#define ECLIPSEMR_FS_IREADER_H_
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "ireader_interface.hh"
#include "../../messages/message.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../../common/context.hh"
#include "../fs/directorymr.hh"

using std::vector;
using std::ifstream;
using std::string;
using std::multimap;
using boost::asio::ip::tcp;

namespace eclipse {

class IReader: public IReader_interface {
 public:
  IReader();
  IReader(uint32_t net_id, uint32_t job_id, uint32_t map_id,
      uint32_t reducer_id);
  ~IReader();

  void init() override;
  void set_net_id(uint32_t net_id) override;
  void set_job_id(uint32_t job_id) override;
  void set_map_id(uint32_t map_id) override;
  void set_reducer_id(uint32_t recducer_id) override;
  bool get_next_key(string &key) override;
  bool get_next_value(string &value) override;
  bool is_next_key() override;
  bool is_next_value() override;

 private:
//  tcp::socket* connect(uint32_t net_id);
//  void send_message(tcp::socket *socket, messages::Message *msg);
//  messages::IGroupInfo* read_igroup_info(tcp::socket *socket);
//  messages::IBlockInfo* read_iblock_info(tcp::socket *socket);
  uint32_t get_num_block();
  void SetNext();
  void SetNextAsCurrent();
  bool ShiftToNextKey();
  bool LoadKey(const int &index);
  bool LoadValue(const int &index);
  bool FinishBlock(const int &index);

  DirectoryMR directory_;
  boost::asio::io_service io_service_;
  string scratch_path_;
  uint32_t net_id_;
  uint32_t job_id_;
  uint32_t map_id_;
  uint32_t reducer_id_;
  uint32_t num_block_;
  multimap<string, int>::iterator get_min_iterator();
  uint32_t num_finished_;
  bool is_next_key_;
  bool is_next_value_;
  bool is_clear_;
  vector<ifstream*> blocks_;
  vector<string> loaded_keys_;
  vector<int> num_remain_;
  int next_block_index_;
  int curr_block_index_;
  string curr_key_;
  string next_key_;
  string next_value_;
  multimap<string, int> key_order_;
  multimap<string, int>::iterator next_it_;
};

}  // namespace eclipse
#endif  // ECLIPSEMR_FS_IREADER_H_
