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

void Application::work()
{
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
      Connect(address);
    }
  }
}

void Application::CreateServer(const std::string port = "5555") {
  auto server_func = [this](const std::string port) { server->Work(port); };
  std::thread th_server(server_func, port);
  th_server.detach();
}

void Application::Connect(const std::string address = "tcp://localhost:5555") {
  auto client_func = [this](const std::string address) { client->Work(address); };
  std::thread th_client(client_func, address);
  th_client.detach();
}
