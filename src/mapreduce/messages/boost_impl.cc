#include "boost_impl.hh"

#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using eclipse::messages::Message;

namespace boost {
namespace serialization {

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Task& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.subjob_id);
    ar & BOOST_SERIALIZATION_NVP(c.id);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.input_path);
    ar & BOOST_SERIALIZATION_NVP(c.func_name);
    ar & BOOST_SERIALIZATION_NVP(c.file_output);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.leader);
    ar & BOOST_SERIALIZATION_NVP(c.func_body);
    ar & BOOST_SERIALIZATION_NVP(c.lang);
    ar & BOOST_SERIALIZATION_NVP(c.pre_map);
    ar & BOOST_SERIALIZATION_NVP(c.after_map);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IDataInsert& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_reducer);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IGroupInsert& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.reducer_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IBlockInsert& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.reducer_id);
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::IDataList& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);

  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::IDataInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c); 
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_reducer);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IDataInfoRequest& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IGroupInfoRequest& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.reducer_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IBlockInfoRequest& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.reducer_id);
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::KeyValueShuffle& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id_);
    ar & BOOST_SERIALIZATION_NVP(c.map_id_);
    ar & BOOST_SERIALIZATION_NVP(c.kv_pairs);
    ar & BOOST_SERIALIZATION_NVP(c.number_of_keys);
    ar & BOOST_SERIALIZATION_NVP(c.kv_id);
    ar & BOOST_SERIALIZATION_NVP(c.node_id);
    ar & BOOST_SERIALIZATION_NVP(c.origin_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::FinishShuffle& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id_);
    ar & BOOST_SERIALIZATION_NVP(c.map_id_);
    ar & BOOST_SERIALIZATION_NVP(c.node_id);
    ar & BOOST_SERIALIZATION_NVP(c.kv_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::TaskStatus& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.is_success);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.saved_keys);
    ar & BOOST_SERIALIZATION_NVP(c.shuffled_nodes);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Job& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.map_name);
    ar & BOOST_SERIALIZATION_NVP(c.reduce_name);
    ar & BOOST_SERIALIZATION_NVP(c.files);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.file_output);
    ar & BOOST_SERIALIZATION_NVP(c.func_body);
    ar & BOOST_SERIALIZATION_NVP(c.lang);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::JobStatus& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.is_success);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IDataKeys& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.keys);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::FinishMap& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.nodes);
  }


template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::NodesShuffling& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.nodes);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.id);
    ar & BOOST_SERIALIZATION_NVP(c.kv_id);
  }

using namespace eclipse::messages;
using namespace boost::archive;

template void serialize (boost::archive::xml_oarchive&, Task&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Task&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Job&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Job&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Job&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Job&, unsigned);

template void serialize (boost::archive::xml_oarchive&, JobStatus&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  JobStatus&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  JobStatus&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  JobStatus&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IDataKeys&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IDataKeys&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IDataKeys&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IDataKeys&, unsigned);

template void serialize (boost::archive::xml_oarchive&, KeyValueShuffle&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  KeyValueShuffle&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  KeyValueShuffle&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  KeyValueShuffle&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FinishShuffle&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FinishShuffle&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FinishShuffle&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FinishShuffle&, unsigned);

template void serialize (boost::archive::xml_oarchive&, TaskStatus&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  TaskStatus&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  TaskStatus&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  TaskStatus&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IDataInsert&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IDataInsert&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IDataInsert&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IDataInsert&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IGroupInsert&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IGroupInsert&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IGroupInsert&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IGroupInsert&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IDataInfoRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IDataInfoRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IDataInfoRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IDataInfoRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IGroupInfoRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IGroupInfoRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IGroupInfoRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IGroupInfoRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IBlockInfoRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IBlockInfoRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IBlockInfoRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IBlockInfoRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IDataList&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IDataList&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IDataList&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IDataList&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IDataInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IDataInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IDataInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IDataInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FinishMap&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FinishMap&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FinishMap&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FinishMap&, unsigned);

template void serialize (boost::archive::xml_oarchive&, NodesShuffling&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  NodesShuffling&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  NodesShuffling&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  NodesShuffling&, unsigned);

}
}


// 4) Also here
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Task);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Job);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::JobStatus);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IDataKeys);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::KeyValueShuffle);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FinishShuffle);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::TaskStatus);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IDataInsert);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IGroupInsert);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IBlockInsert);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IDataInfoRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IGroupInfoRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IBlockInfoRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IDataList);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IDataInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FinishMap);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::NodesShuffling);
