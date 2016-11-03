#include "dataset.hh"
#include "../messages/boost_impl.hh"
#include "../messages/factory.hh"
#include "../messages/job.hh"
#include "../common/ecfs.hh"
#include "../common/hash.hh"
#include <vector>
#include <iomanip>
#include <random>

using namespace eclipse;
using namespace std;
using namespace eclipse::messages;
using vec_str = std::vector<std::string>;

// Network {{{
//template <typename T>
//auto read_reply(tcp::socket* socket) {
//  using namespace boost::asio;
//  char header[17] = {0};
//  header[16] = '\0';
//  boost::asio::streambuf buf;
//
//  read(*socket, buffer(header, 16));
//  size_t size_of_msg = atoi(header);
//
//  read(*socket, buf, transfer_exactly(size_of_msg));
//
//  Message* msg = nullptr;
//  msg = load_message(buf);
//  T* m = dynamic_cast<T*>(msg);
//  return unique_ptr<T>(m);
//}
//
//void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
//  string*  out = save_message(msg);
//  socket->send(boost::asio::buffer(*out));
//}
//// }}}

DataSet& DataSet::open (std::string in) {
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 
      std::numeric_limits<uint32_t>::max());

  uint32_t id = dist(rng);
  auto data = new DataSet(id);
  data->file = in;
  return *data;
}

DataSet::DataSet (uint32_t id_) : 
  job_id (id_), 
  socket (iosvc) 
{
  auto  ep = find_local_master();
  socket.connect(*ep);
}

tcp::endpoint* DataSet::find_local_master() {
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.ports.client");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ job_id % nodes.size() ];

  cout << "[CLIENT] submitting Job id: "<< job_id << " to LEADER " << job_id % nodes.size() << endl;
  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);

  return ep;
}

void DataSet::map (std::string func) {
  Job job;
  job.type = "MAP";
  job.library = "libfoo.so";
  job.map_name = func;
  job.files.push_back(file);
  job.job_id = job_id;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
}

void DataSet::reduce (std::string func, std::string output) {
  Job job;
  job.type = "REDUCE";
  job.library = "libfoo.so";
  job.reduce_name = func;
  job.files.push_back(file);
  job.job_id = job_id;
  job.file_output = output;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
}
