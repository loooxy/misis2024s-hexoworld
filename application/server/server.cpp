#include "server.hpp"
#include <zmqhelper/zmqhelper.hpp>

const int REQUEST_CYCLES = 10;

const int HEARTBEAT_INTERVAL = 1000;   //  msecs
const int HEARTBEAT_LIVENESS = 3;   //  3-5 is reasonable

const int MAX_EVENT_ID = 1000000000;

// if server received heartbeat from client, it updates his time
void client_refresh(std::map<std::string, time_point> id_to_time, const std::string& identity) {
  if (id_to_time.count(identity) == 0) {
    std::cout << "E: client " << identity << " not ready" << std::endl;
  }
  else {
    id_to_time[identity] = Clock::msc_clock();
  }
}

// if server didn't receive heartbeat from client, it deletes him
void client_purge(std::map<std::string, time_point> id_to_time) {
  time_point time = Clock::msc_clock();
  for (auto it = id_to_time.cbegin(); it != id_to_time.cend(); ) {
    if (Clock::elapsed(it->second, time) > HEARTBEAT_INTERVAL * HEARTBEAT_LIVENESS) {
      id_to_time.erase(it++);
    }
    else {
      it++;
    }
  }
}

Server::Server() 
  : 
  ctx_(1), 
  server_(ctx_, zmq::socket_type::router)
{
  backend_ = std::make_unique<Backend>();
}

Server::~Server() {

}

// push events in queues for every client
void Server::UpdateEventsQueue() {
  int event_id = 0;
  std::unique_lock<std::mutex> lock(id_to_queue_mtx, std::defer_lock);
  while (true) {
    std::string ev;
    backend_->GetDataToReply(ev);

    if (ev.size() > 0) {
      for (auto& pair : id_to_queue) {
        lock.lock();
        pair.second.push(EventId(ev, event_id));
        lock.unlock();
      }

      event_id = (event_id + 1) % MAX_EVENT_ID;
    }
  }
}

void Server::CreateServer(const std::string port = "5555") {
  server_.bind("tcp://*:" + port);
}

void Server::Work(const std::string port) {
  auto run_func = [this](std::string port) {Run(port); };
  std::thread th_run(run_func, port);

  auto update_events_func = [this]() {UpdateEventsQueue(); };
  std::thread th_update_events(update_events_func);

  backend_->work();
  th_run.detach();
  th_update_events.detach();
}

void Server::Run(const std::string port){
  CreateServer(port);

  auto heartbeat_prev = Clock::msc_clock();

  while (true) {
    // server receive requests from clients
    zmq::pollitem_t items[] = {
      {server_, 0, ZMQ_POLLIN, 0}
    };

    zmq::poll(&items[0], 1, HEARTBEAT_INTERVAL);

    // if we got a request, reply
    if (items[0].revents & ZMQ_POLLIN) {
      zmq::message_t identity;
      zmq::message_t delimiter;
      zmq::message_t request;
      // receive identity
      server_.recv(identity, zmq::recv_flags::none);
      // receive delimiter
      server_.recv(delimiter, zmq::recv_flags::none);
      // receive request
      server_.recv(request, zmq::recv_flags::none);

      // init id_to_cam, id_to_queue
      std::string id = identity.to_string();
      if (id_to_cam.count(id) == 0) {
        id_to_cam[id] = Camera();
        id_to_queue[id].empty();
        id_to_time[id] = Clock::msc_clock();
      }
      std::string req_str = request.to_string();
      // update client timer
      if (req_str == "HEARTBEAT") {
        client_refresh(id_to_time, id);
      }
      // first request, need to reply with map
      else if (req_str == "FR") {
        zmq::message_t reply_map;
        zmq::message_t reply_map_basis;
        FillReplyMap(reply_map);
        FillReplyMapBasis(reply_map_basis);
        ZmqHelper::send_id(server_, identity);
        ZmqHelper::send_empty(server_);
        server_.send(reply_map, zmq::send_flags::sndmore);
        server_.send(reply_map_basis, zmq::send_flags::none);
      }
      // confirmation of event
      else if (req_str.size() == 5) {
        ConfirmEvent(id, req_str);
      }
      // process event from client
      else {
        ProcessRequest(request);
      }
    }

    // send heartbeat when time is up
    auto time = Clock::msc_clock();
    if (Clock::elapsed(heartbeat_prev, time) > HEARTBEAT_INTERVAL) {
      for (const auto& pair : id_to_time) {
        zmq::message_t identity(pair.first);
        ZmqHelper::send_id(server_, identity);
        ZmqHelper::send_empty(server_);
        server_.send(zmq::message_t(std::string("HEARTBEAT")), zmq::send_flags::none);
      }
      heartbeat_prev = Clock::msc_clock();
    }

    // send events to all clients from their queues
    for (const auto& pair : id_to_queue) {
      zmq::message_t reply_event;
      FillReplyEvent(reply_event, pair.first);
      if (reply_event.size() > 0) {
        zmq::message_t identity(pair.first);
        ZmqHelper::send_id(server_, identity);
        ZmqHelper::send_empty(server_);
        server_.send(reply_event, zmq::send_flags::none);
      }
    }

    client_purge(id_to_time);
  }
}


void Server::FillReplyEvent(zmq::message_t& reply_event, const std::string& id) {
  std::lock_guard<std::mutex> lock(id_to_queue_mtx);
  if (!id_to_queue[id].empty()) {
    std::string event_id = saveEventId(id_to_queue[id].front());
    reply_event.rebuild(event_id.data(), sizeof(event_id[0]) * event_id.size());
  }
}

void Server::FillReplyMap(zmq::message_t& reply_map) {
  std::string map;
  backend_->GetMap(map);
  reply_map.rebuild(map.data(), sizeof(map[0]) * map.size());
}

void Server::FillReplyMapBasis(zmq::message_t& reply_map_basis) {
  std::string map_basis;
  backend_->GetMapBasis(map_basis);
  reply_map_basis.rebuild(map_basis.data(), sizeof(map_basis[0]) * map_basis.size());
}


void Server::ProcessRequest(zmq::message_t& request) {
  std::string ev = request.to_string();
  backend_->ProcessData(ev);
}

void Server::ConfirmEvent(const std::string& id, const std::string& event_id) {
  std::lock_guard<std::mutex> lock(id_to_queue_mtx);
  Id id_event = loadId(event_id);
  while (!id_to_queue[id].empty() && id_to_queue[id].front().id != id_event.id) {
    id_to_queue[id].pop();
  }
  if (!id_to_queue[id].empty()) {
    id_to_queue[id].pop();
  }
}