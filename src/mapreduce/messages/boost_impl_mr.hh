#pragma once
#include "../../messages/boost_impl.hh"
#include "idatainsert.hh"
#include "igroupinsert.hh"
#include "iblockinsert.hh"
#include "idatainforequest.hh"
#include "igroupinforequest.hh"
#include "iblockinforequest.hh"

namespace eclipse {
namespace messages {

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
    ar & BOOST_SERIALIZATION_NVP(c.job_id);
    ar & BOOST_SERIALIZATION_NVP(c.map_id);
  }

}  // namespace messages
}  // namespace eclipse

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
BOOST_CLASS_TRACKING(eclipse::messages::KeyValueShuffle,
    boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FinishShuffle,
    boost::serialization::track_never);
