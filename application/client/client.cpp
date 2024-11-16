#include <application/application.hpp>
#define REQUEST_TIMEOUT 100
#define REQUEST_RETRIES 3
#define IDENTITY_SIZE 5

// define data type
enum DataType {
  VERTICES,
  TRILIST,
  CAMERAS
};

Client::Client()
  :
  ctx_(1),
  client_(ctx_, ZMQ_REQ)
{}

Client::~Client() {

}

void Client::Work() {
  auto frontend_func = [this]() {frontend_->work(); };
  std::thread th_frontend(frontend_func);

  th_frontend.detach();

  ConnectToServer("tcp://*:5555");
}

void Client::ConnectToServer(const std::string& address = "tcp://*:5555") {
  client_.connect(address);
  frontend_->SetIsClient(true);

  int retries_left = REQUEST_RETRIES;

  while (retries_left) {
    // client fill request with commands
    zmq::message_t request;
    FillRequest(request);

    // client send request
    client_.send(request);

    bool expect_reply = true;
    while (expect_reply) {
      zmq::pollitem_t items[] = {
        {client_, 0, ZMQ_POLLIN, 0}
      };

      zmq::poll(&items[0], 1, REQUEST_TIMEOUT);

      // if we got a reply
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t reply_vertices;
        zmq::message_t reply_trilist;
        //zmq::message_t reply_cameras;
        client_.recv(reply_vertices, zmq::recv_flags::none);
        client_.recv(reply_trilist, zmq::recv_flags::none);
        //client_.recv(reply_cameras, zmq::recv_flags::none);

        // forward map data to application
        ForwardDataToApp(reply_vertices, reply_trilist);

        // forward camera data to application

      }
      else if (--retries_left == 0){
        std::cout << "E: server seems to be offline, abandoning" << std::endl;
        expect_reply = false;
        break;
      }
      else {
        std::cout << "W: no response from server, retrying..." << std::endl;
        client_.close();
        client_ = zmq::socket_t(ctx_, zmq::socket_type::req);

        FillRequest(request);
        client_.send(request);
      }
    }
  }
}

// fill request with data about commands
void Client::FillRequest(zmq::message_t& request) {
  std::string ev;
  frontend_->GetDataToRequest(ev);
  request.rebuild(ev.data(), sizeof(ev[0]) * ev.size());
}

// forward data to application
void Client::ForwardDataToApp(zmq::message_t& reply_vertices, zmq::message_t& reply_trilist) {
    // vertices
    std::vector<PrintingPoint> Vertices(reply_vertices.size() / sizeof(PrintingPoint));
    memcpy(Vertices.data(), reply_vertices.data(), reply_vertices.size());

    // trilist
    std::vector<uint16_t> TriList(reply_trilist.size() / sizeof(uint16_t));
    memcpy(TriList.data(), reply_trilist.data(), reply_trilist.size());

    // forward to app
    frontend_->SetDataFromReply(Vertices, TriList);

    // cameras
    //std::vector<std::pair<std::string, Camera>> v(reply_cameras.size() / (IDENTITY_SIZE + sizeof(Camera)));
    //memcpy(v.data(), reply_cameras.data(), reply_cameras.size());

    //// TODO: add drawing other cameras
    //std::map<std::string, Camera> id_to_cam;
    //for (const auto& pair : v) {
    //  id_to_cam[pair.first] = pair.second;
    //}
}