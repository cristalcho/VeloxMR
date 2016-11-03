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
#include "filelist.hh"
#include "blockinfo.hh"
#include "reply.hh"
#include "cacheinfo.hh"
#include "filerequest.hh"
#include "blockrequest.hh"
#include "filedescription.hh"
#include "blockdel.hh"
#include "filedel.hh"
#include "formatrequest.hh"
#include "fileexist.hh"
#include "blockupdate.hh"
#include "fileupdate.hh"
#include "metadata.hh"

// MapReduce messages
#include "../mapreduce/messages/idatalist.hh"
#include "../mapreduce/messages/idatainfo.hh"
#include "../mapreduce/messages/idatainsert.hh"
#include "../mapreduce/messages/igroupinsert.hh"
#include "../mapreduce/messages/iblockinsert.hh"
#include "../mapreduce/messages/idatainforequest.hh"
#include "../mapreduce/messages/igroupinforequest.hh"
#include "../mapreduce/messages/iblockinforequest.hh"
#include "../mapreduce/messages/key_value_shuffle.h"
#include "../mapreduce/messages/finish_shuffle.h"
#include "taskstatus.hh"
#include "offsetkv.hh"
#include "job.hh"
#include "jobstatus.hh"
#include "subjob.hh"
#include "subjobstatus.hh"
#include "idatakeys.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/base_object.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif
#define ECNS eclipse::messages

//! 2) Also here
namespace boost{
namespace serialization{

template <typename Archive> void serialize (Archive&, eclipse::messages::Message&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Boundaries&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyValue&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::OffsetKeyValue&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Control&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Task&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileUpdate&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileList&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockUpdate&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Reply&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::CacheInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDescription&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDel&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockDel&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FormatRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileExist&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::MetaData&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Job&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::JobStatus&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::SubJob&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::SubJobStatus&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataKeys&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyValueShuffle&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FinishShuffle&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::TaskStatus&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IGroupInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IBlockInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataInfoRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IGroupInfoRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IBlockInfoRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataList&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataInfo&, unsigned);
}
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(ECNS::Message);

#define TRACK_NEVER  boost::serialization::track_never
//! 3) Also here
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Boundaries);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyValue);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::OffsetKeyValue);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Control);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Task);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockUpdate);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::CacheInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockDel);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::MetaData);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Job);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::JobStatus);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::SubJob);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::SubJobStatus);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataKeys);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyValueShuffle);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FinishShuffle);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::TaskStatus);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IGroupInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IBlockInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataInfoRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IGroupInfoRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IBlockInfoRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataList);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataInfo);

//! 4) and here
BOOST_CLASS_TRACKING(eclipse::messages::Message, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Boundaries, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::OffsetKeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Control, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Task, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Reply, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::CacheInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDescription, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FormatRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileExist, boost::serialization::track_never);
BOOST_CLASS_TRACKING(ECNS::IDataInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IBlockInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IBlockInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::KeyValueShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FinishShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::TaskStatus, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Job, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::JobStatus, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::SubJob, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::SubJobStatus, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataKeys, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataList, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInfo, TRACK_NEVER);
#undef ECNS
#undef TRACK_NEVER 
