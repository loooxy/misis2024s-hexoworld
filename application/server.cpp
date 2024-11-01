#include <application/application.hpp>
#define REQUEST_TIMEOUT 10
#define REQUEST_CYCLES 10
#define IDENTITY_SIZE 5

// define data type
enum DataType {
  VERTICES,
  TRILIST,
  CAMERAS
};

Application::Server::Server(Application* app) 
  : app_(app), 
  ctx_(1), 
  server_(ctx_, ZMQ_ROUTER)
{}

Application::Server::~Server() {

}

void Application::Server::CreateServer(int port = 5555) {
  server_.bind("tcp://*:" + port);
}

void Application::Server::Run() {
  while (true) {
    // server receive requests from clients
    for (int i = 0; i < REQUEST_CYCLES; ++i) {
      zmq::pollitem_t items[] = {
        {server_, 0, ZMQ_POLLIN, 0}
      };

      zmq::poll(&items[0], 1, REQUEST_TIMEOUT);

      // if we got a request, reply
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t request;
        zmq::message_t identity;
        zmq::message_t delimiter;
        // receive identity
        server_.recv(identity, zmq::recv_flags::none);
        // receive delimiter
        server_.recv(delimiter, zmq::recv_flags::none);
        // receive request
        server_.recv(request, zmq::recv_flags::none);

        // server can process request here


        // change id_to_cam
        std::string id = identity.to_string();
        if (id_to_cam.count(id) == 0) {
          id_to_cam[id] = Camera();
        }

        // fill reply with data
        zmq::message_t reply_vertices;
        zmq::message_t reply_trilist;
        zmq::message_t reply_cameras;
        FillReply(reply_vertices, reply_trilist, reply_cameras);

        // send identity
        server_.send(identity, zmq::send_flags::sndmore);
        // send delimiter
        server_.send(delimiter, zmq::send_flags::sndmore);
        // send reply
        server_.send(reply_vertices, zmq::send_flags::sndmore);
        server_.send(reply_trilist, zmq::send_flags::sndmore);
        server_.send(reply_cameras, zmq::send_flags::none);
      }
    }

    // server can process requests here 
  }
}

// TODO: using class data get vertices and trilist
// fill reply with data about map (and cameras)
void Application::Server::FillReply(zmq::message_t& reply_vertices, zmq::message_t& reply_trilist, zmq::message_t& reply_cameras) {
  // maybe we can use zero-copy, but not now
  
  // vertices
  reply_vertices.rebuild(Application::Frontend::GetVertices().data(), sizeof(Application::Frontend::GetVertices()[0]) * Application::Frontend::GetVertices().size());

  // trilist
  reply_trilist.rebuild(Application::Frontend::GetTriList().data(), sizeof(Application::Frontend::GetTriList()[0]) * Application::Frontend::GetTriList().size());

  // cameras
  std::vector<std::pair<std::string, Camera>> v;
  for (const auto& pair : id_to_cam) {
    v.push_back({ pair.first, pair.second });
  }

  reply_cameras.rebuild(v.data(), (IDENTITY_SIZE + sizeof(Camera)) * v.size());

}