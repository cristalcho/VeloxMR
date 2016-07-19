#pragma once
#include "../common/hash.hh"
#include "../common/context_singleton.hh"
#include "../common/histogram.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockupdate.hh"
#include "../messages/fileexist.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/blockdel.hh"
#include "../messages/filedescription.hh"
#include "../messages/reply.hh"
#include "../messages/blockrequest.hh"
#include "directory.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>
#include <boost/asio.hpp>
#include <stdint.h>

namespace eclipse {
  using namespace messages;
  using boost::asio::ip::tcp;
  enum class FILETYPE {
    Normal,
    App,
    Idata
  };
  class DFS {
    private:
      uint32_t BLOCK_SIZE;
      uint32_t NUM_NODES;
      std::string path;
      boost::asio::io_service& iosvc;
      int replica;
      int port;
      std::vector<std::string> nodes;

      unique_ptr<tcp::socket> connect (uint32_t);
      void send_message(tcp::socket*, eclipse::messages::Message*);
      template <typename T>
      auto read_reply(tcp::socket*);


    public:
      DFS();
      void load_settings ();
      int put(int argc, char* argv[]);
      int get(int argc, char* argv[]);
      int cat(int argc, char* argv[]);
      int ls(int argc, char* argv[]);
      int rm(int argc, char* argv[]);
      int format(int argc, char* argv[]);
      int show(int argc, char* argv[]);
      int pget(int argc, char* argv[]);
      int update(int argc, char* argv[]);
      int append(int argc, char* argv[]);
  };
}
