#include <application/application.hpp>
#include <thread>
#include <iostream>

Application::Application() 
{
  client = std::make_shared<Client>();
  server = std::make_shared<Server>();
}

Application::~Application()
{
}

void Application::work() {
  zmq::context_t context;

  auto read_func = [this](zmq::context_t& context) {ReadCommands(std::ref(context)); };
  std::thread th_read(read_func, std::ref(context));

  client->Work(context);

  th_read.detach();
}

void Application::ReadCommands(zmq::context_t& context) {
  zmq::socket_t xmitter_client(context, zmq::socket_type::pair);
  xmitter_client.connect("inproc://client");
  while (true) {
    std::string address;
    std::string port;
    std::string action;

    std::cin >> action;

    if (action == "Create") {
      std::cout << "Enter port: ";
      std::cin >> port;
      CreateServer(port);
    }
    if (action == "Connect") {
      std::cout << "Enter address: ";
      std::cin >> address;
      zmq::message_t msg(address);
      xmitter_client.send(msg, zmq::send_flags::none);
    }
    if (action == "Disconnect") {
      zmq::message_t msg(action);
      xmitter_client.send(msg, zmq::send_flags::none);
    }
    if (action == "Exit") {
      zmq::message_t msg(action);
      xmitter_client.send(msg, zmq::send_flags::none);
    }
  }
}

void Application::CreateServer(const std::string port = "5555") {
  auto server_func = [this](const std::string port) { server->Work(port); };
  std::thread th_server(server_func, port);
  th_server.detach();
}
