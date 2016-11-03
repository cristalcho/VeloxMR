#include "boost_impl.hh"

#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using eclipse::messages::Message;

namespace boost {
namespace serialization {

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Message& m, unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(m.origin);
    ar & BOOST_SERIALIZATION_NVP(m.destination);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Boundaries& b, unsigned int) {
    ar & BASE_OBJECT(Message, b);
    ar & BOOST_SERIALIZATION_NVP(b.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyValue& k, unsigned int) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::OffsetKeyValue& k, unsigned int) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
    ar & BOOST_SERIALIZATION_NVP(k.pos);
    ar & BOOST_SERIALIZATION_NVP(k.len);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Control& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileUpdate& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name); 
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.seq);  
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.size);     
    ar & BOOST_SERIALIZATION_NVP(c.type);   
    ar & BOOST_SERIALIZATION_NVP(c.replica);   
    ar & BOOST_SERIALIZATION_NVP(c.node);        
    ar & BOOST_SERIALIZATION_NVP(c.l_node);      
    ar & BOOST_SERIALIZATION_NVP(c.r_node);      
    ar & BOOST_SERIALIZATION_NVP(c.is_committed);  
    ar & BOOST_SERIALIZATION_NVP(c.content);  
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockUpdate& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);  
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.seq);  
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.size);     
    ar & BOOST_SERIALIZATION_NVP(c.replica);   
    ar & BOOST_SERIALIZATION_NVP(c.content);  
    ar & BOOST_SERIALIZATION_NVP(c.pos);     
    ar & BOOST_SERIALIZATION_NVP(c.len);   
    ar & BOOST_SERIALIZATION_NVP(c.is_header);   
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Task& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.subjob_id);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.input_path);
    ar & BOOST_SERIALIZATION_NVP(c.func_name);
    ar & BOOST_SERIALIZATION_NVP(c.file_output);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.leader);
  }
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileList& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Reply& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.message);
    ar & BOOST_SERIALIZATION_NVP(c.details);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::CacheInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.keys);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.hash_keys);
    ar & BOOST_SERIALIZATION_NVP(c.block_size);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.seq);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FormatRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileExist& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MetaData& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.node);
    ar & BOOST_SERIALIZATION_NVP(c.content);
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
    ar & BOOST_SERIALIZATION_NVP(c.key_);
    ar & BOOST_SERIALIZATION_NVP(c.value_);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::FinishShuffle& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id_);
    ar & BOOST_SERIALIZATION_NVP(c.map_id_);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::TaskStatus& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.is_success);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.subjob_id);
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
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::JobStatus& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.is_success);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::SubJob& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.map_name);
    ar & BOOST_SERIALIZATION_NVP(c.reduce_name);
    ar & BOOST_SERIALIZATION_NVP(c.file);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::SubJobStatus& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.is_success);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.subjob_id);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IDataKeys& c,
      unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.keys);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
  }


using namespace eclipse::messages;
using namespace boost::archive;

template void serialize<xml_oarchive>   (boost::archive::xml_oarchive&, Message&, unsigned);
template void serialize<xml_iarchive>   (boost::archive::xml_iarchive&,  Message&, unsigned);
template void serialize<binary_iarchive>(boost::archive::binary_iarchive&, Message&, unsigned);
template void serialize<binary_oarchive>(boost::archive::binary_oarchive&, Message&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Boundaries&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Boundaries&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Boundaries&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Boundaries&, unsigned);

template void serialize (boost::archive::xml_oarchive&, KeyValue&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  KeyValue&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  KeyValue&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  KeyValue&, unsigned);

template void serialize (boost::archive::xml_oarchive&, OffsetKeyValue&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  OffsetKeyValue&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  OffsetKeyValue&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  OffsetKeyValue&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Control&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Control&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Control&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Control&, unsigned);

template void serialize (boost::archive::xml_oarchive&, KeyRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  KeyRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  KeyRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  KeyRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Task&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Task&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileUpdate&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileUpdate&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileUpdate&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileUpdate&, unsigned);

template void serialize<xml_oarchive>   (boost::archive::xml_oarchive&, FileList&, unsigned);
template void serialize<xml_iarchive>   (boost::archive::xml_iarchive&,  FileList&, unsigned);
template void serialize<binary_iarchive>(boost::archive::binary_iarchive&,  FileList&, unsigned);
template void serialize<binary_oarchive>(boost::archive::binary_oarchive&,  FileList&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockUpdate&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockUpdate&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockUpdate&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockUpdate&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Reply&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Reply&, unsigned);

template void serialize (boost::archive::xml_oarchive&, CacheInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  CacheInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  CacheInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  CacheInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileDescription&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileDescription&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileDescription&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileDescription&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileDel&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileDel&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileDel&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileDel&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockDel&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockDel&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockDel&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockDel&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FormatRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FormatRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FormatRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FormatRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileExist&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileExist&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileExist&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileExist&, unsigned);

template void serialize (boost::archive::xml_oarchive&, MetaData&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  MetaData&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  MetaData&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  MetaData&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Job&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Job&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Job&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Job&, unsigned);

template void serialize (boost::archive::xml_oarchive&, JobStatus&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  JobStatus&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  JobStatus&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  JobStatus&, unsigned);

template void serialize (boost::archive::xml_oarchive&, SubJob&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  SubJob&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  SubJob&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  SubJob&, unsigned);

template void serialize (boost::archive::xml_oarchive&, SubJobStatus&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  SubJobStatus&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  SubJobStatus&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  SubJobStatus&, unsigned);

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

}
}


//! 4) Also here
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Boundaries);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::KeyValue);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::OffsetKeyValue);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Control);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::KeyRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Task);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockUpdate);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::CacheInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockDel);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::MetaData);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Job);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::JobStatus);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::SubJob);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::SubJobStatus);
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
