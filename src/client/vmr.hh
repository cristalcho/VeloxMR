#pragma once

#ifdef STANDALONE_VERSION
#include "client/vdfs.hh"
#else
#include <eclipsedfs/vdfs.hh>
#endif


#include <memory>

namespace velox {

class vmr;

class dataset {
  friend vmr;
  public:
    void map(std::string);
    void reduce(std::string);

    void pymap(std::string, std::string, std::string);
    void pyreduce(std::string, std::string);

  protected:
    dataset(vmr*, std::vector<std::string>);

    uint32_t job_id = 0;
    std::vector<std::string> files;
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
