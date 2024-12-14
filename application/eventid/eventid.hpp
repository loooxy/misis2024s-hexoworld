#pragma once
#include <string>
#include <cereal/types/string.hpp>

struct EventId {
  EventId() = default;
  EventId(const EventId&) = default;
  EventId& operator=(const EventId&) = default;
  ~EventId() = default;

  EventId(const std::string& event, const int id) : event(event), id(id) {};

  std::string event;
  int id = 0;

  template<class Archive>
  void serialize(Archive& ar) {
    ar(event, id);
  }
};

struct Id {
  Id() = default;
  Id(const Id&) = default;
  Id& operator=(const Id&) = default;
  ~Id() = default;

  Id(const int id) : id(id) {};

  int id = 0;

  template<class Archive>
  void serialize(Archive& ar) {
    ar(id);
  }
};

EventId loadEventId(const std::string& data);
std::string saveEventId(const EventId& event_id);
Id loadId(const std::string& data);
std::string saveId(const Id& id);
