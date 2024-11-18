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

Backend::Backend() {
  wwm = std::make_shared<WorkWithMap>();
  wwm->get_data(data);
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
    events.lock();
    while (events.empty())
    {
      event = events.pop();
      was_events = true;

      if (event->type() == close)
        break;
      else
        event->execute(wwm);
    }
    events.unlock();

    if (was_events) {
      if (event != nullptr && event->type() == close)
        break;

      // wwm->print_in_vertices_and_triList(Vertices, TriList);

      wwm->get_data(data);
    }
  }
  application_is_alive = false;

  river_update.join();
}

void Backend::regular_event_update_river()
{
  while (application_is_alive)
  {
    events.push(std::make_shared<UpdateRiver>());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Backend::GetDataToReply(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) {
  if (data.check()) {
    data.get(Vertices, TriList);
  }
}

void Backend::ProcessData(std::string& ev) {
  if (!ev.empty()) {
    events.push(loadEv(ev));
  }
}

