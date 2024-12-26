#include "client.hpp"
#include <clock/clock.hpp>
#include <eventid/eventid.hpp>
#include <zmqhelper/zmqhelper.hpp>

const int REQUEST_TIMEOUT = 10000;
const int REQUEST_RETRIES = 3;

const int HEARTBEAT_LIVENESS = 5;   //  3-5 is reasonable
const int HEARTBEAT_INTERVAL = 1000;   //  msecs
const int INTERVAL_INIT = 1000;   //  Initial reconnect
const int INTERVAL_MAX = 32000;    //  After exponential backoff

Client::Client()
  :
  ctx_(1),
  client_(ctx_, zmq::socket_type::dealer)
{
  frontend_ = std::make_unique<Frontend>();
}

Client::~Client() {

}

void Client::ManageSignals(zmq::context_t& context) {
  zmq::socket_t receiver(context, zmq::socket_type::pair);
  zmq::socket_t xmitter(context, zmq::socket_type::pair);
  receiver.bind("inproc://client");
  xmitter.connect("inproc://frontend");
  zmq::message_t message;
  while (true) {
    receiver.recv(message, zmq::recv_flags::none);
    if (message.to_string() == "Disconnect") {
      xmitter.send(message);
      is_connected.store(false);
    }else if (message.to_string() == "Exit") {
      xmitter.send(message);
      is_connected.store(false);
      is_running.store(false);
    }
    else {
      {
        std::lock_guard<std::mutex> connect_lock(connect_mtx);
        is_connected.store(true);
        address_ = message.to_string();
      }
      cv_connect.notify_all();
    }
  }
}

void Client::Work(zmq::context_t& context) {
  while (is_running) {
    auto manage_signals_func = [this](zmq::context_t& context) { ManageSignals(std::ref(context)); };
    std::thread th_manage_signals(manage_signals_func, std::ref(context));

    std::unique_lock<std::mutex> connect_lock(connect_mtx);
    cv_connect.wait(connect_lock, [this] {return is_connected.load(); });

    auto connect_func = [this]() {ConnectToServer(); };
    std::thread th_connect(connect_func);

    std::unique_lock<std::mutex> map_lock(map_mtx);
    cv_map.wait(map_lock, [this] {return is_map_received.load(); });

    frontend_->work(context);
    th_connect.join();
  }
}

void Client::ConnectToServer() {
  client_.connect(address_);

  // first request for map
  RequestMap();

  {
    std::lock_guard<std::mutex> map_lock(map_mtx);
    is_map_received.store(true);
  }
  cv_map.notify_all();

  int liveness = HEARTBEAT_LIVENESS;
  int interval = INTERVAL_INIT;

  auto heartbeat_prev = Clock::msc_clock();

  while (is_connected.load()) {
    zmq::pollitem_t items[] = {
      {client_, 0, ZMQ_POLLIN, 0}
    };
    zmq::poll(&items[0], 1, HEARTBEAT_INTERVAL);

    if (items[0].revents & ZMQ_POLLIN) {
      zmq::message_t delimiter;
      client_.recv(delimiter, zmq::recv_flags::none);

      zmq::message_t reply;
      zmq::message_t type;
      client_.recv(type, zmq::recv_flags::none);

      std::string type_str = type.to_string();

      if (type_str == "HEARTBEAT") {
        liveness = HEARTBEAT_LIVENESS;
      }
      else  if (type_str == "EVENT") {
        // receive event
        client_.recv(reply, zmq::recv_flags::none);
        int id = ForwardEventToApp(reply);
        // send confirmimation of receiving event
        ZmqHelper::send_empty(client_);
        std::string id_str = saveId(Id(id));
        client_.send(zmq::message_t(std::string("CONFIRM")), zmq::send_flags::sndmore);
        client_.send(zmq::message_t(id_str), zmq::send_flags::none);
        liveness = HEARTBEAT_LIVENESS;
      }
      else if (type_str == "CAMERAS") {
        // receive cameras
        client_.recv(reply, zmq::recv_flags::none);
        ForwardCamerasToApp(reply);
        liveness = HEARTBEAT_LIVENESS;
      }
      else {
        std::cout << "E: invalid message from server" << std::endl;
      }
    }
    // reconnect if liveness = 0
    else if (--liveness == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(interval));

      if (interval < INTERVAL_MAX) {
        interval *= 2;
      }

      client_.close();
      client_ = zmq::socket_t(ctx_, zmq::socket_type::dealer);
      client_.connect(address_);
      RequestMap();
    }

    // send heartbeat when time is up
    auto time = Clock::msc_clock();
    if (Clock::elapsed(heartbeat_prev, time) > HEARTBEAT_INTERVAL) {
      heartbeat_prev = time;
      ZmqHelper::send_empty(client_);
      client_.send(zmq::message_t(std::string("HEARTBEAT")), zmq::send_flags::none);
    }

    // send event if have
    zmq::message_t request;
    FillRequestEvent(request);
    if (request.size() > 0) {
      ZmqHelper::send_empty(client_);
      client_.send(zmq::message_t(std::string("EVENT")), zmq::send_flags::sndmore);
      client_.send(request, zmq::send_flags::none);
    }

    // send command if have
    FillRequestCommand(request);
    if (request.size() > 0) {
      ZmqHelper::send_empty(client_);
      client_.send(zmq::message_t(std::string("COMMAND")), zmq::send_flags::sndmore);
      client_.send(request, zmq::send_flags::none);
    }
  }
}

// fill request with data about events
void Client::FillRequestEvent(zmq::message_t& event) {
  std::string ev;
  frontend_->GetEventToRequest(ev);
  event.rebuild(ev.data(), sizeof(ev[0]) * ev.size());
}

void Client::FillRequestCommand(zmq::message_t& command) {
  std::string com;
  frontend_->GetCommandToRequest(com);
  command.rebuild(com.data(), sizeof(com[0]) * com.size());
}

// forward event to application
int Client::ForwardEventToApp(zmq::message_t& reply_event) {
  std::string data = reply_event.to_string();
  EventId event_id = loadEventId(data);
  frontend_->ProcessEvent(event_id.event);

  return event_id.id;
}

// forward cameras to app
void Client::ForwardCamerasToApp(zmq::message_t& reply_cameras) {
  std::string data = reply_cameras.to_string();
  frontend_->ProcessCameras(data);
}

void Client::RequestMap() {
  int retries_left = REQUEST_RETRIES;

  while (retries_left) {
    // client fill request for map
    zmq::message_t request(std::string("FR"));

    // client send request
    ZmqHelper::send_empty(client_);
    client_.send(request);

    zmq::pollitem_t items[] = {
      {client_, 0, ZMQ_POLLIN, 0}
    };

    zmq::poll(&items[0], 1, REQUEST_TIMEOUT);

    // if we got a reply
    if (items[0].revents & ZMQ_POLLIN) {
      zmq::message_t reply_map;
      zmq::message_t reply_map_basis;
      zmq::message_t delimiter;
      client_.recv(delimiter, zmq::recv_flags::none);
      client_.recv(reply_map, zmq::recv_flags::none);
      client_.recv(reply_map_basis, zmq::recv_flags::none);

      // forward map data to application
      ForwardMapToApp(reply_map, reply_map_basis);
      break;
    }
    else if (--retries_left == 0) {
      std::cout << "E: server seems to be offline, abandoning" << std::endl;
      break;
    }
    else {
      std::cout << "W: no response from server, retrying..." << std::endl;
      client_.close();
      client_ = zmq::socket_t(ctx_, zmq::socket_type::dealer);
      client_.connect(address_);
    }
  }
}

void Client::ForwardMapToApp(zmq::message_t& reply_map, zmq::message_t& reply_map_basis) {
  std::string map = reply_map.to_string();
  std::string map_basis = reply_map_basis.to_string();
  frontend_->ProcessMap(map);
  frontend_->ProcessMapBasis(map_basis);
}