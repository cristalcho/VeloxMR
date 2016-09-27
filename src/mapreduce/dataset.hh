#pragma once
#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using boost::asio::ip::tcp;

class DataSet {
  public:
    void map(std::string);
    void reduce(std::string, std::string);
    static DataSet& open (std::string);

  private:
    DataSet (uint32_t);
    uint32_t job_id = 0;
    std::string file;
    tcp::endpoint* find_local_master();
    boost::asio::io_service iosvc;
    tcp::socket socket;
};

}
