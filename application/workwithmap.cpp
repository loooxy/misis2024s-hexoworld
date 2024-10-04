#include "application.hpp"
#include "application.hpp"
#include <application/application.hpp>

void Application::WorkWithMap::generateField(std::shared_ptr<Hexoworld>& map) {
  map = std::make_shared<Hexoworld>(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(0, 0, -2), Eigen::Vector3d(-1, 0, 0), 1, 2, 10, 10);

  Eigen::Matrix<int, 10, 10> colors_ = (Eigen::MatrixXi(10, 10) <<
    mo, mo, sn, sn, sn, mo, mo, se, se, se,
    gr, gr, mo, mo, mo, sa, mo, se, se, se,
    sa, sa, gr, gr, gr, sa, sa, gr, se, se,
    mo, gr, sa, sa, sa, gr, sa, gr, gr, gr,
    mo, gr, gr, sa, sa, sa, gr, sa, sa, gr,
    mo, mo, gr, se, se, se, gr, sa, gr, gr,
    mo, mo, se, se, se, sa, gr, gr, sa, gr,
    mo, mo, se, se, se, sa, gr, gr, se, se,
    mo, mo, gr, gr, se, sa, gr, gr, se, sa,
    sn, mo, mo, mo, gr, gr, gr, gr, sa, sa).finished();

  Eigen::Matrix<int, 10, 10> heights_ = (Eigen::MatrixXi(10, 10) <<
    3, 3, 4, 4, 4, 3, 2, -1, -2, -3,
    1, 1, 2, 2, 3, 1, 2, -1, -2, -2,
    0, 0, 1, 1, 1, 0, 0, 1, -1, -1,
    2, 1, 0, 0, 0, 1, 0, 1, 1, 0,
    3, 1, 1, 1, 0, 0, 1, 0, 0, 1,
    2, 3, 0, -1, -1, -1, 1, 0, 1, 1,
    3, 2, -1, -2, -2, 0, 0, 1, 0, 1,
    3, 2, -1, -1, -1, 0, 1, 0, -1, -2,
    2, 3, 1, 0, -1, 0, 1, 0, -1, 1,
    4, 3, 3, 2, 0, 1, 1, 0, 1, 1).finished();

  map->add_river({
                    {0, 3}, {1, 2}, {2, 3}, {3, 2}, {3, 3}, {4, 4}, {5, 4}
    });
  map->road(4, 6) = true;
  map->road(3, 5) = true;
  map->road(3, 6) = true;

  map->farm(4, 9) = true;

  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
    {
      switch (colors_(i, j))
      {
      case gr: map->color(i, j) = grass; break;
      case sa: map->color(i, j) = sand; break;
      case se: map->color(i, j) = sea; break;
      case sn: map->color(i, j) = snow; break;
      case mo: map->color(i, j) = mount; break;
      case te: map->color(i, j) = test; break;
      }

      map->height(i, j) = heights_(i, j);
    }
}

void Application::WorkWithMap::regular_event_update_river()
{
  while (application_is_alive)
  {
    app->events.push(std::make_shared<UpdateRiver>(app));

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

  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  map->print_in_vertices_and_triList(Vertices, TriList);
  app->data.set(Vertices, TriList);
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
    app->events.lock();
    while (!app->events.empty())
    {
      event = app->events.pop();

      if (event->type() == close)
        break;
      else
        event->execute(this);
    }
    app->events.unlock();
    
    if (event != nullptr && event->type() == close)
      break;

    map->print_in_vertices_and_triList(Vertices, TriList);

    app->data.set(Vertices, TriList);
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
int Application::WorkWithMap::get_id_color(Eigen::Vector4i color)
{
  if (color == sea)
    return se;
  if (color == sand)
    return sa;
  if (color == grass)
    return gr;
  if (color == mount)
    return mo;
  if (color == snow)
    return sn;
  if (color == test)
    return te;
}

int Application::WorkWithMap::heights(int row, int col)
{
  return map->height(row, col);
}

int Application::WorkWithMap::colors(int row, int col)
{
  return get_id_color(map->color(row, col));
}

bool Application::WorkWithMap::roads(int row, int col)
{
  return map->road(row, col);
}

bool Application::WorkWithMap::farms(int row, int col)
{
  return map->farm(row, col);
}

bool Application::WorkWithMap::flood(int row, int col)
{
  return map->flood(row, col);
}

void Application::WorkWithMap::set_hex_height(int row, int col, int new_height)
{
  map->height(row, col) = new_height;
}

void Application::WorkWithMap::set_hex_color(int row, int col, int color_id)
{
  map->color(row, col) = get_color(color_id);
}

void Application::WorkWithMap::set_road_state_in_hex(int row, int col, bool road_state)
{
  map->road(row, col) = road_state;
}

void Application::WorkWithMap::set_farm_state_in_hex(int row, int col, bool farm_state)
{
  map->farm(row, col) = farm_state;
}

void Application::WorkWithMap::set_flood_state_in_hex(int row, int col, bool flood_state)
{
  map->flood(row, col) = flood_state;
}

void Application::WorkWithMap::update_river()
{
  map->update_river();
}

