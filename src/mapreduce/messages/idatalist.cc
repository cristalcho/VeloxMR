#include "idatalist.hh"

namespace eclipse {
namespace messages {

IDataList::IDataList (std::vector<IDataInfo> v) : data(v) { }
std::string IDataList::get_type() const {return "IDataList"; }

}
}
