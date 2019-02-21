#include "vmr.hh"
#include "common/context_singleton.hh"
#include "../messages/boost_impl.hh"
#include "../messages/factory.hh"
#include "../mapreduce/messages/job.hh"
#include "common/hash.hh"
#include <vector>
#include <iomanip>
#include <random>
#include <cstdlib>
#include <boost/asio.hpp>

using namespace std;
using namespace velox;
using namespace eclipse::messages;
using namespace boost::asio;
using namespace boost::asio::ip;
using vec_str = std::vector<std::string>;

// Free functions {{{
uint32_t random_number() {
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 
      std::numeric_limits<uint32_t>::max());

  return dist(rng);
}

tcp::endpoint* find_local_master(uint32_t job_id) {

  int port      = GET_INT("network.ports.mapreduce");
  vec_str nodes = GET_VEC_STR("network.nodes");

  string host = nodes[ job_id % nodes.size() ];

  //cout << "[CLIENT] submitting Job id: "<< job_id << " to LEADER " << job_id % nodes.size() << endl;
  tcp::resolver resolver (context.io);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);

  return ep;
}

std::string base_name(std::string const & path) {
  return path.substr(path.find_last_of("/\\") + 1);
}
// }}}
// Constructors {{{
dataset::dataset(vmr* vmr_, std::vector<std::string> files) {
  this->vmr_ = vmr_;
  this->files = files;
  this->job_id = random_number();

  //socket->connect(*find_local_master(job_id));
}
//}}}
// map {{{
void dataset::map(std::string func) {
  tcp::socket socket (context.io);
  socket.connect(*find_local_master(job_id));

  Job job;
  job.type = "MAP";
  job.lang = "C++";
  job.library = base_name(getenv("_")) + ".so";
  job.map_name = func;
  job.files = files;
  job.job_id = job_id;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
  socket.close();
}
// }}}
// sj_reduce {{{
void dataset::reduce(std::string func){
  tcp::socket socket (context.io);
  socket.connect(*find_local_master(job_id));

  Job job;
  job.type = "REDUCE";
  job.lang = "C++";
  job.library = base_name(getenv("_")) + ".so";
  job.reduce_name = func;
  job.job_id = job_id;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
  socket.close();
}
// }}}
// pymap {{{
void dataset::pymap(std::string func, std::string pmap = "", std::string amap = "") {
  tcp::socket socket (context.io);
  socket.connect(*find_local_master(job_id));

  Job job;
  job.type = "MAP";
  job.lang = "Python";
  job.files = files;
  job.job_id = job_id;
  job.func_body = func;
  job.before_map = pmap;
  job.after_map = amap;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
  socket.close();
}
// }}}
// pyreduce {{{
void dataset::pyreduce(std::string func, std::string output) {
  tcp::socket socket (context.io);
  socket.connect(*find_local_master(job_id));

  Job job;
  job.type = "REDUCE";
  job.lang = "Python";
  job.files = files;
  job.job_id = job_id;
  job.func_body = func;
  job.file_output = output;

  send_message(&socket, &job);
  auto reply = read_reply<Reply> (&socket);
  socket.close();
}
// }}}
// Constructor vmr {{{
vmr::vmr(vdfs* vdfs_) {
  this->vdfs_ = vdfs_;
}
// }}}
// make_dataset {{{
dataset vmr::make_dataset(std::vector<std::string> files) {
  return dataset(this, files);
}
// }}}
