#include <application/application.hpp>

void Application::WorkWithMap::generateField(std::shared_ptr<Hexoworld>& map) {
  map = std::make_shared<Hexoworld>(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(0, 0, -2), Eigen::Vector3d(-1, 0, 0), 1, 2, 10, 10);

  map->add_river({
                    {0, 3}, {1, 2}, {2, 3}, {3, 2}, {3, 3}, {4, 4}, {5, 4}
    });
  map->add_road_in_hex(4, 6);
  map->add_road_in_hex(3, 5);
  map->add_road_in_hex(3, 6);

  map->add_farm_in_hex(4, 9);

  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
    {
      switch (colors_(i, j))
      {
      case gr: map->set_hex_color(i, j, grass); break;
      case sa: map->set_hex_color(i, j, sand); break;
      case se: map->set_hex_color(i, j, sea); break;
      case sn: map->set_hex_color(i, j, snow); break;
      case mo: map->set_hex_color(i, j, mount); break;
      case te: map->set_hex_color(i, j, test); break;
      }

      map->set_hex_height(i, j, heights_(i, j));
    }
}

void Application::WorkWithMap::regular_event_update_river()
{
  while (application_is_alive)
  {
    app->events_mtx.lock();
    
    app->events.push(std::make_shared<UpdateRiver>(app));
    
    app->events_mtx.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

Application::WorkWithMap::WorkWithMap(Application* app)
  : app(app),
  grass(53, 200, 45, 255),
  sand(252, 221, 50, 255),
  sea(0, 100, 255, 255),
  snow(255, 255, 255, 255),
  mount(181, 184, 177, 255),
  test(219, 255, 255, 255)
{
  elems_names[0] = "grass";
  elems_names[1] = "sand";
  elems_names[2] = "sea";
  elems_names[3] = "snow";
  elems_names[4] = "mount";
  elems_names[5] = "test";

  generateField(map);

  n_rows = map->get_n_rows();
  n_cols = map->get_n_cols();

  map->print_in_vertices_and_triList(app->Vertices, app->TriList);
}

Application::WorkWithMap::~WorkWithMap()
{
}

void Application::WorkWithMap::work()
{
  auto river_update_func = [this]() { regular_event_update_river(); };
  std::thread river_update(river_update_func);

  std::shared_ptr<Event> event = nullptr;

  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  while (true)
  {
    app->events_mtx.lock();

    while (!app->events.empty())
    {
      event = app->events.front();
      app->events.pop();

      if (event->type() == close)
        break;
      else
        event->execute(this);
    }

    app->events_mtx.unlock();
    
    if (event != nullptr && event->type() == close)
      break;

    map->print_in_vertices_and_triList(Vertices, TriList);

    app->buffers_mtx.lock();
    std::swap(app->Vertices, Vertices);
    std::swap(app->TriList, TriList);
    app->buffers_mtx.unlock();
  }
  application_is_alive = false;

  river_update.join();
}

int Application::WorkWithMap::get_n_cols()
{
  return n_cols;
}

int Application::WorkWithMap::get_n_rows()
{
  return n_rows;
}

Eigen::Vector4i Application::WorkWithMap::get_color(int color_id)
{
  switch (color_id)
  {
  case se: return sea;
  case sa: return sand;
  case gr: return grass;
  case mo: return mount;
  case sn: return snow;
  case te: return test;
  }
}

int Application::WorkWithMap::heights(int row, int col)
{
  return heights_(row, col);
}

int Application::WorkWithMap::colors(int row, int col)
{
  return colors_(row, col);
}

bool Application::WorkWithMap::roads(int row, int col)
{
  return roads_(row, col);
}

bool Application::WorkWithMap::farms(int row, int col)
{
  return farms_(row, col);
}

void Application::WorkWithMap::set_hex_height(int row, int col, int new_height)
{
  map->set_hex_height(row, col, new_height);
  heights_(row, col) = new_height;
}

void Application::WorkWithMap::set_hex_color(int row, int col, int color_id)
{
  map->set_hex_color(row, col, get_color(color_id));
  colors_(row, col) = color_id;
}

void Application::WorkWithMap::set_road_state_in_hex(int row, int col, bool road_state)
{
  if (road_state)
    map->add_road_in_hex(row, col);
  else
    map->del_road_in_hex(row, col);

  roads_(row, col) = road_state;
}

void Application::WorkWithMap::set_farm_state_in_hex(int row, int col, bool farm_state)
{
  if (farm_state)
    map->add_farm_in_hex(row, col);
  else
    map->del_farm_in_hex(row, col);

  farms_(row, col) = farm_state;
}

void Application::WorkWithMap::update_river()
{
  map->update_river();
}

