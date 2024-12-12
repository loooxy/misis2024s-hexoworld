#include "frontend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>

std::string saveEv(const std::shared_ptr<Event>& ev) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(ev);
  return oss.str();
}

std::shared_ptr<Event> loadEv(const std::string& data) {
  std::shared_ptr<Event> ev;
  std::istringstream iss(data);
  cereal::PortableBinaryInputArchive archive(iss);
  archive(ev);
  return ev;
}

void Frontend::work() {
  std::shared_ptr<Event> event = nullptr;

  bool was_events = false;
  while (true)
  {
    was_events = false;
    events.lock();
    while (!events.empty())
    {
      event = events.pop();
      was_events = true;

      if (event->type() == close)
        break;
      else
        render_->UpdateMap(event);
    }
    events.unlock();

    if (was_events) {
      if (event != nullptr && event->type() == close)
        break;

      render_->UpdateData();
    }
  }
}


void Frontend::ProcessEvent(std::string& ev) {
  if (!ev.empty()) {
    events.push(loadEv(ev));
  }
}

void Frontend::GetDataToRequest(std::string& ev) {
  std::shared_ptr<Event> event = render_->GetEvent();
  if (event != nullptr) {
    ev = saveEv(event);
  }
}

Frontend::Frontend() 
{
  render_ = std::make_unique<Render>();
}

void Frontend::ProcessMap(std::string& map) {
  render_->InitMap(map);
}
void Frontend::ProcessMapBasis(std::string& map_basis) {
  render_->InitMapBasis(map_basis);
}
