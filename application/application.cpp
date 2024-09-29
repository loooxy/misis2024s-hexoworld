#include <application/application.hpp>

Application::Application() 
{
  work_with_map = std::make_shared<WorkWithMap>(this);
  frontend = std::make_shared<Frontend>(this);
}

Application::~Application()
{
}

void Application::work()
{
  auto wwm_func = [this]() { work_with_map->work(); };
  
  std::thread wwm(wwm_func);
 
  frontend->work();
  
  wwm.join();
}
