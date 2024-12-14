#include "backend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>


std::string saveMap(const std::shared_ptr<WorkWithMap>& map) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(map);
  return oss.str();
}

std::string saveMapBasis(const MapBasis& map_basis) {
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
