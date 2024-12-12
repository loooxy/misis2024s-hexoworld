#include "backend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>

std::shared_ptr<Event> loadEv(const std::string& data) {
  std::shared_ptr<Event> ev;
  std::istringstream iss(data);
  cereal::PortableBinaryInputArchive archive(iss);
  archive(ev);
  return ev;
}

std::string saveEv(const std::shared_ptr<Event>& ev) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(ev);
  return oss.str();
}

std::string saveMap(const std::shared_ptr<WorkWithMap>& map) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(map);
  return oss.str();
}

std::string saveMapBasis(const MapBasis map_basis) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(map_basis);
  return oss.str();
}

Backend::Backend() {
  wwm = std::make_shared<WorkWithMap>();
}

void Backend::work()
{
  auto river_update_func = [this]() { regular_event_update_river(); };
  std::thread river_update(river_update_func);

  std::shared_ptr<Event> event = nullptr;

  bool was_events = false;
  while (true)
  {
    was_events = false;
    events_in.lock();
    while (!events_in.empty())
    {
      event = events_in.pop();
      was_events = true;

      events_out.push(event);
      if (event->type() == close) {
        break;
      }
      else {
        event->execute(wwm);
      }
    }
    events_in.unlock();

    if (was_events) {
      if (event != nullptr && event->type() == close)
        break;

    }
  }
  application_is_alive = false;

  river_update.join();
}

void Backend::regular_event_update_river()
{
  while (application_is_alive)
  {
    events_in.push(std::make_shared<UpdateRiver>());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Backend::GetDataToReply(std::string& ev) {
  if (!events_out.empty()) {
    ev = saveEv(events_out.pop());
  }
}

void Backend::ProcessData(std::string& ev) {
  if (!ev.empty()) {
    events_in.push(loadEv(ev));
  }
}

void Backend::GetMap(std::string& map) {
  map = saveMap(wwm);
}

void Backend::GetMapBasis(std::string& map_basis) {
  map_basis = saveMapBasis(wwm->get_map_basis());
}
