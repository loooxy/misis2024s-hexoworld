#include "eventid.hpp"
#include <cereal/archives/portable_binary.hpp>

EventId loadEventId(const std::string& data) {
  EventId event_id;
  std::istringstream iss(data);
  cereal::PortableBinaryInputArchive archive(iss);
  archive(event_id);
  return event_id;
}

std::string saveEventId(const EventId& event_id) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(event_id);
  return oss.str();
}

Id loadId(const std::string& data) {
  Id id;
  std::istringstream iss(data);
  cereal::PortableBinaryInputArchive archive(iss);
  archive(id);
  return id;
}

std::string saveId(const Id& id) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(id);
  return oss.str();
}