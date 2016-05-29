#include "dataset.hh"
#include "../messages/boost_impl.hh"
#include "../messages/factory.hh"
#include "../common/ecfs.hh"
#include "../common/hash.hh"
#include <vector>
#include <iomanip>
#include <random>

using namespace eclipse;
using namespace std;
using namespace eclipse::messages;
using vec_str = std::vector<std::string>;


template <typename T>
T* read_reply(tcp::socket* socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  eclipse::messages::Message* m = load_message(recv_msg);
  delete[] body;
  return dynamic_cast<T*>(m);
}

void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;

  socket->send(boost::asio::buffer(ss.str()));
}

DataSet& DataSet::open (std::string in) {
  uint32_t id_ = h(in);
  auto data = new DataSet(id_);
  data->file = in;
  return *data;
}

DataSet::DataSet (uint32_t id_) : 
  id (id_), 
  socket (iosvc) 
{
  find_local_master();
  auto  ep = find_local_master();
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 
      std::numeric_limits<uint32_t>::max());
  job_id = dist(rng);
  cout << "[CLIENT] submitting Job id: " << job_id << endl;

  socket.connect(*ep);
}

tcp::endpoint* DataSet::find_local_master() {
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.ports.client");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ id % nodes.size() ];

  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);

  return ep;
}

DataSet& DataSet::map (std::string func) {
  Task map_task;
  map_task.library = "libfoo.so";
  map_task.func_name = func;
  map_task.input_path = file;
  map_task.type = "MAP";
  map_task.job_id = job_id;
  map_task.map_id = 0;

  send_message(&socket, &map_task);
  auto reply = read_reply<Reply> (&socket);
  return *(new DataSet(2131231));
}

DataSet& DataSet::reduce (std::string func) {
  Task task;
  task.library = "libfoo.so";
  task.func_name = func;
  task.input_path = file;
  task.type = "REDUCE";
  task.job_id = job_id;

  send_message(&socket, &task);
  auto reply = read_reply<Reply> (&socket);
  return *(new DataSet(2131231));
}
