//
// This module implements the serialization using BOOST SERIALIZAITON
// For every new message class that you want to add you must add its 
// functions here and its cc file.
//
#pragma once

//! 1) Add your class here
#include "message.hh"
#include "boundaries.hh"
#include "keyvalue.hh"
#include "control.hh"
#include "keyrequest.hh"
#include "task.hh"
#include "fileinfo.hh"
#include "../mapreduce/messages/idatainfo.hh"
#include "filelist.hh"
#include "../mapreduce/messages/idatalist.hh"
#include "blockinfo.hh"
#include "reply.hh"
#include "cacheinfo.hh"
#include "filerequest.hh"
#include "blockrequest.hh"
#include "filedescription.hh"
#include "blockdel.hh"
#include "filedel.hh"
#include "formatrequest.hh"
#include "../mapreduce/messages/idatainsert.hh"
#include "../mapreduce/messages/igroupinsert.hh"
#include "../mapreduce/messages/iblockinsert.hh"
#include "../mapreduce/messages/idatainforequest.hh"
#include "../mapreduce/messages/igroupinforequest.hh"
#include "../mapreduce/messages/iblockinforequest.hh"
#include "../mapreduce/messages/key_value_shuffle.h"
#include "../mapreduce/messages/finish_shuffle.h"
#include "fileexist.hh"
#include "taskstatus.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif
#define ECNS eclipse::messages

//! 2) Also here
namespace eclipse {
namespace messages {

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
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.file_hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.file_size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
 }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::IDataInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c); 
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_reducer);
  }
  
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);  
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);  
    ar & BOOST_SERIALIZATION_NVP(c.block_hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.block_size);     
    ar & BOOST_SERIALIZATION_NVP(c.is_inter);   
    ar & BOOST_SERIALIZATION_NVP(c.node);        
    ar & BOOST_SERIALIZATION_NVP(c.l_node);      
    ar & BOOST_SERIALIZATION_NVP(c.r_node);      
    ar & BOOST_SERIALIZATION_NVP(c.is_committed);  
    ar & BOOST_SERIALIZATION_NVP(c.content);  
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Task& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.input_path);
    ar & BOOST_SERIALIZATION_NVP(c.func_name);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
  }
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileList& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::IDataList& c, unsigned int) {
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
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.hash_keys);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);
    ar & BOOST_SERIALIZATION_NVP(c.block_hash_key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FormatRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
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
  void serialize (Archive& ar, eclipse::messages::FileExist& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
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
  }

}  // namespace messages
}  // namespace eclipse


BOOST_SERIALIZATION_ASSUME_ABSTRACT(ECNS::Message);

#define TRACK_NEVER  boost::serialization::track_never
//! 3) Also here
BOOST_CLASS_TRACKING(ECNS::Message, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Boundaries, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::KeyValue, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Control, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::KeyRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileInfo, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInfo, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::BlockInfo, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Task, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileList, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataList, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Reply, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::CacheInfo, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::BlockRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileDescription, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileDel, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::BlockDel, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FormatRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IBlockInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IBlockInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FileExist, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::KeyValueShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FinishShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::TaskStatus, TRACK_NEVER);

#undef ECNS
#undef TRACK_NEVER 
