#include "frontend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>

void Frontend::work(zmq::context_t& context) {
  auto river_update_func = [this]() { regular_event_update_river(); };
  std::thread th_river_update(river_update_func);

  auto handle_func = [this]() { HandleEvents(); };
  std::thread th_handle(handle_func);

  auto manage_signals_func = [this](zmq::context_t& context) { ManageSignals(std::ref(context)); };
  std::thread th_manage_signals(manage_signals_func, std::ref(context));

  render_->work();

  th_river_update.join();
  th_handle.join();
  th_manage_signals.join();
}

void Frontend::ManageSignals(zmq::context_t& context) {
  zmq::socket_t receiver(context, zmq::socket_type::pair);
  receiver.bind("inproc://frontend");
  zmq::message_t message;
  while (application_is_alive.load()) {
    receiver.recv(message, zmq::recv_flags::none);
    if (message.to_string() == "Disconnect") {
      render_->Stop();
      application_is_alive.store(false);
    }
  }
}

void Frontend::HandleEvents() {
  std::shared_ptr<Event> event = nullptr;

  bool was_events = false;
  while (application_is_alive.load())
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
      if (event != nullptr && event->type() == close) {
        application_is_alive.store(false);
        break;
      }

      render_->UpdateData();
    }
  }
}

void Frontend::regular_event_update_river()
{
  while (application_is_alive.load())
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

void Frontend::ProcessCameras(std::string& cameras) {
  if (!cameras.empty()) {
    render_->UpdateCameras(cameras);
  }
}

void Frontend::GetEventToRequest(std::string& ev) {
  std::shared_ptr<Event> event = render_->GetEvent();
  if (event != nullptr) {
    ev = saveEv(event);
  }
}

void Frontend::GetCommandToRequest(std::string& com) {
  std::shared_ptr<Command> command = render_->GetCommand();
  if (command != nullptr) {
    com = saveCommand(command);
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
