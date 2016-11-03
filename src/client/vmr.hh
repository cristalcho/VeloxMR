#pragma once
#include "vdfs.hh"
#include <memory>

namespace velox {

class vmr;

class dataset {
  friend vmr;
  public:
    void map(std::string);
    void reduce(std::string, std::string);

  protected:
    dataset(vmr*, std::vector<std::string>);

    uint32_t job_id = 0;
    std::vector<std::string> files;
    //std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    vmr* vmr_ = nullptr;
};

class vmr {
  public:
    vmr(vdfs*);
    dataset make_dataset(std::vector<std::string>);

  protected:
    vdfs* vdfs_ = nullptr;
};

}
