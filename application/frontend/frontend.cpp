#include "frontend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>

void Frontend::work() {
  auto river_update_func = [this]() { regular_event_update_river(); };
  std::thread th_river_update(river_update_func);

  auto render_func = [this]() { render_->work(); };
  std::thread th_render(render_func);

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
  application_is_alive = false;

  th_river_update.join();
  th_render.detach();
}

void Frontend::regular_event_update_river()
{
  while (application_is_alive)
  {
    events.push(std::make_shared<UpdateRiver>());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

Frontend::~Frontend() {

}

void Frontend::ProcessMap(std::string& map) {
  render_->InitMap(map);
}
void Frontend::ProcessMapBasis(std::string& map_basis) {
  render_->InitMapBasis(map_basis);
}
