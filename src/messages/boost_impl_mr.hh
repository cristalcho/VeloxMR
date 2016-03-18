#pragma once
#include "boost_impl.hh"
#include "idatainfo.hh"
#include "igroupinfo.hh"
#include "iblockinfo.hh"
#include "idatainforequest.hh"
#include "igroupinforequest.hh"
#include "iblockinforequest.hh"

namespace eclipse {
namespace messages {

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IDataInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_reducer);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IGroupInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
    ar & BOOST_SERIALIZATION_NVP(c.reducer_id);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
  }

template <typename Archive>
  void serialize(Archive& ar, eclipse::messages::IBlockInfo& c, unsigned int) {
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

}
}

BOOST_CLASS_TRACKING(eclipse::messages::IDataInfo,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IGroupInfo,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IBlockInfo,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IDataInfoRequest,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IGroupInfoRequest,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IBlockInfoRequest,
    boost::serialization::track_never);
