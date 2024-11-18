#include <application/application.hpp>
#include <thread>

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
  auto server_func = [this]() { server->Run(); };
  
  server->CreateServer(5555);

  std::thread th_server(server_func);
 
  client->Work();
  
  th_server.detach();
}
