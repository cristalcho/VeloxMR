#pragma once

#include "../../messages/message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "../mapreduce/messages/idatainfo.hh"

namespace eclipse {
namespace messages {

struct IDataList: public Message {
  IDataList() = default;
  IDataList(std::vector<IDataInfo>);

  std::string get_type() const override;
  std::vector<IDataInfo> data;
};

}
}
