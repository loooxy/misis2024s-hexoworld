#pragma once
#include <zmq.hpp>
#include <string>

class ZmqHelper {
public:
  static void send_empty(zmq::socket_t& socket) {
    socket.send(zmq::message_t(std::string("")), zmq::send_flags::sndmore);
  }

  static void send_id(zmq::socket_t& socket, zmq::message_t& id) {
    socket.send(id, zmq::send_flags::sndmore);
  }
};
