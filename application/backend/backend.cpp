#include "backend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
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

std::string saveCameras(const std::map<std::string, Camera>& id_to_cam) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(id_to_cam);
  return oss.str();
}

Backend::Backend() {
  wwm = std::make_shared<WorkWithMap>();
}

void Backend::work()
{
  std::shared_ptr<Event> event = nullptr;
  std::pair<std::string, std::shared_ptr<Command>> id_com;

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

    std::lock_guard<std::mutex> lock(commands_mtx);
    while (!commands.empty()) {
      id_com = commands.front();
      commands.pop();

      id_com.second->execute(id_to_cam[id_com.first]);
    }
  }

}

void Backend::GetDataToReply(std::string& ev) {
  if (!events_out.empty()) {
    ev = saveEv(events_out.pop());
  }
}

void Backend::ProcessEvent(std::string& ev) {
  if (!ev.empty()) {
    events_in.push(loadEv(ev));
  }
}

void Backend::ProcessCommand(std::string& com, const std::string& id) {
  std::lock_guard<std::mutex> lock(commands_mtx);
  if (!com.empty()) {
    commands.push({id, loadCommand(com) });
  }
}

void Backend::GetMap(std::string& map) {
  map = saveMap(wwm);
}

void Backend::GetMapBasis(std::string& map_basis) {
  map_basis = saveMapBasis(wwm->get_map_basis());
}

void Backend::GetCameras(std::string& cameras) {
  if (!id_to_cam.empty()) {
    cameras = saveCameras(id_to_cam);
  }
}

void Backend::DeleteCamera(const std::string id) {
  if (id_to_cam.count(id) != 0) {
    id_to_cam.erase(id);
  }
}
