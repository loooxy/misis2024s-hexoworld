#include "frontend.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <sstream>

const int WAIT_TIME = 1000;

void Frontend::work() {
  auto river_update_func = [this]() { regular_event_update_river(); };
  std::thread th_river_update(river_update_func);

  auto handle_func = [this]() { HandleEvents(); };
  std::thread th_handle(handle_func);

  render_->work();

  th_river_update.join();
  th_handle.join();

  render_.reset();
  events.clear();
}

void Frontend::ManageSignals(zmq::context_t& context) {
  zmq::socket_t receiver(context, zmq::socket_type::pair);
  receiver.bind("inproc://frontend");
  zmq::message_t message;
  while (application_is_alive.load()) {
    zmq::pollitem_t items[] = {
     {receiver, 0, ZMQ_POLLIN, 0}
    };
    zmq::poll(&items[0], 1, WAIT_TIME);

    if (items[0].revents & ZMQ_POLLIN) {
      receiver.recv(message, zmq::recv_flags::none);
      if (message.to_string() == "Disconnect") {
        render_->Stop();
        is_running.store(false);
      }
      else if (message.to_string() == "Connect") {
        is_running.store(true);
      }
      else if (message.to_string() == "Exit") {
        render_->Stop();
        is_running.store(false);
        application_is_alive.store(false);
      }
    }
  }
}

void Frontend::InitRender() {
  render_ = std::make_unique<Render>();
}

void Frontend::HandleEvents() {
  std::shared_ptr<Event> event = nullptr;

  bool was_events = false;
  while (is_running.load())
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
        is_running.store(false);
        break;
      }

      render_->UpdateData();
    }
  }
}

void Frontend::regular_event_update_river()
{
  while (is_running.load())
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
  if (render_ == nullptr) {
    return;
  }
  if (!cameras.empty()) {
    render_->UpdateCameras(cameras);
  }
}

void Frontend::GetEventToRequest(std::string& ev) {
  if (render_ == nullptr) {
    return;
  }
  std::shared_ptr<Event> event = render_->GetEvent();
  if (event == nullptr) {
    return;
  }
  if (event->type() == close) {
    events.push(event);
  }
  else {
    ev = saveEv(event);
  }
}

void Frontend::GetCommandToRequest(std::string& com) {
  if (render_ == nullptr) {
    return;
  }
  std::shared_ptr<Command> command = render_->GetCommand();
  if (command != nullptr) {
    com = saveCommand(command);
  }
}

Frontend::Frontend() 
{
}

Frontend::~Frontend() {
}

void Frontend::ProcessMap(std::string& map) {
  render_->InitMap(map);
}
void Frontend::ProcessMapBasis(std::string& map_basis) {
  render_->InitMapBasis(map_basis);
}
