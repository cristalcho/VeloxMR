#pragma once
#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using boost::asio::ip::tcp;

class DataSet {
  public:
    DataSet& map(std::string);
    DataSet& reduce(std::string);
    static DataSet& open (std::string);

  private:
    DataSet (uint32_t);
    int id;
    std::string file;
    tcp::endpoint* find_local_master();
    boost::asio::io_service iosvc;
    tcp::socket socket;
};

}
