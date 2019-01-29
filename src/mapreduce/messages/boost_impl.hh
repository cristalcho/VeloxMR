//
//! @brief This module implements the serialization using BOOST SERIALIZAITON
//! For every new message class that you want to add you must add its 
//! functions here and its cc file.
//
#pragma once

// 1) Add your class here
#include "messages/boost_impl.hh"

#include "idatalist.hh"
#include "idatainfo.hh"
#include "idatainsert.hh"
#include "igroupinsert.hh"
#include "iblockinsert.hh"
#include "igroupinforequest.hh"
#include "key_value_shuffle.h"
#include "finish_shuffle.h"
#include "taskstatus.hh"
#include "job.hh"
#include "finishmap.hh"
#include "nodes_shuffling.hh"
#include "task.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/base_object.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif
#define ECNS eclipse::messages

// 2) Also here
namespace boost{
namespace serialization{

template <typename Archive> void serialize (Archive&, eclipse::messages::Task&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::TaskStatus&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Job&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyValueShuffle&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FinishShuffle&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IGroupInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IBlockInsert&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IGroupInfoRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataList&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IDataInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::NodesShuffling&, unsigned);
}
}

//BOOST_SERIALIZATION_ASSUME_ABSTRACT(ECNS::Message);

#define TRACK_NEVER  boost::serialization::track_never
// 3) Also here
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Task);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Job);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyValueShuffle);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FinishShuffle);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::TaskStatus);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IGroupInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IBlockInsert);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IGroupInfoRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataList);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IDataInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FinishMap);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::NodesShuffling);

// 4) and here
BOOST_CLASS_TRACKING(ECNS::Task, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::TaskStatus, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IBlockInsert, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IGroupInfoRequest, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::KeyValueShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FinishShuffle, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::Job, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataList, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::IDataInfo, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::FinishMap, TRACK_NEVER);
BOOST_CLASS_TRACKING(ECNS::NodesShuffling, TRACK_NEVER);

#undef ECNS
#undef TRACK_NEVER 
