#include "hexoworld.hpp"
#include "hexoworld.hpp"
#include <hexoworld/hexoworld.hpp>
#include <hexoworld/manager/manager.hpp>
#include <hexoworld/base_objects/hexagon/hexagon.hpp>
#include <hexoworld/base_objects/rectangle/rectangle.hpp>
#include <hexoworld/base_objects/triangle/triangle.hpp>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

#ifdef PARALLEL
#include <thread>
#endif // PARALLEL

#ifdef SPEED_TEST
#include <iostream>
#include <chrono>
#endif // SPEED_TEST


const double Hexoworld::PRECISION_DBL_CALC = 0.0000001;

Hexoworld::Hexoworld(
  float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, 
  Eigen::Vector3d сol_direction, 
  float height_step,
  uint32_t n_terraces_on_height_step,
  uint32_t n_rows, uint32_t n_cols)
  : origin_(origin), 
  rowDirection_(row_direction.normalized()), 
  colDirection_(сol_direction.normalized()),
  heightDirection_(row_direction.cross(сol_direction).normalized()),
  size_(size), 
  heightStep_(height_step), 
  nTerracesOnHeightStep_(n_terraces_on_height_step),
  n_rows(n_rows),
  n_cols(n_cols)
{
  if (abs(rowDirection_.dot(colDirection_)) > PRECISION_DBL_CALC)
    throw std::invalid_argument(
      "row_direction and сol_direction not perpendicular");

  manager = std::make_unique<Manager>(*this);

  for (int row = 0; row < n_rows; ++row)
  {
    for (int col = 0; col < n_cols; ++col)
    {
      add_hexagon(row, col,
        Eigen::Vector4i(255, 255, 255, 0));
    }
  }
}

void Hexoworld::add_hexagon(uint32_t row, uint32_t col,
  Eigen::Vector4i color)
{
  Coord pos(row, col);
  
  std::shared_ptr<Hexagon> hex = manager->get_hexagon(pos);
  std::static_pointer_cast<Hexagon::UsualDrawer>(hex->drawers.at(Usual))->set_color(color);

  std::set<Coord> neighbors = manager->get_neighbors(pos);
  for (Coord pos_neighbor : neighbors)
  {
    manager->add_rectangle(pos, pos_neighbor);

    std::vector<Coord> triangles_pos;
    std::set<Coord> neighbors_second = manager->get_neighbors(pos_neighbor);

    std::set_intersection(
      neighbors.begin(), neighbors.end(),
      neighbors_second.begin(), neighbors_second.end(),
      std::back_inserter(triangles_pos));

    for (Coord pos_third : triangles_pos)
      manager->add_triangle(pos, pos_neighbor, pos_third);
  }
}

void Hexoworld::add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs) {
  std::vector<Coord> positions;
  for (const auto& hex : hexs)
    positions.push_back(Coord(hex.first, hex.second));

  if (positions.size() <= 1)
    throw std::invalid_argument("So short river.");

  for (int i = 0; i < positions.size() - 1; ++i)
  {
    const std::set<Coord>& neighbors = manager->get_neighbors(positions[i]);
    if (neighbors.find(positions[i + 1]) == neighbors.end())
      throw std::invalid_argument("Cell " + std::to_string(i) + " and cell " + std::to_string(i + 1) + 
      " are not neighbors.");
  }

  std::vector<std::shared_ptr<Object>> river;
  for (int i = 0; i < positions.size(); ++i)
  {
    if (i > 0)
    {
      river.push_back(manager->get_rectangle(positions[i], positions[i - 1]));
      manager->get_rectangle(positions[i], positions[i - 1])->make_river();
    }

    uint32_t before_ind, next_ind;
    before_ind = (i > 0                    ? get_ind_direction(positions[i], positions[i - 1]) : -1);
    next_ind   = (i < positions.size() - 1 ? get_ind_direction(positions[i], positions[i + 1]) : -1);

    river.push_back(manager->get_hexagon(positions[i]));
    manager->get_hexagon(positions[i])->make_river(before_ind, next_ind);
  }

  manager->add_river(river);
}

void Hexoworld::flood_hex(uint32_t row, uint32_t col)
{
  manager->get_hexagon(Coord(row, col))->make_flooding();
}

void Hexoworld::add_road_in_hex(uint32_t row, uint32_t col)
{
  Coord pos(row, col);
  std::set<Coord> neighbors = manager->get_neighbors(pos);

  std::vector<uint32_t> directions;
  for (auto neighbor : neighbors) 
  {
    std::shared_ptr<Hexagon> hex = manager->get_hexagon(neighbor);
    if (hex->frames.find(Road) != hex->frames.end())
    {
      directions.push_back(get_ind_direction(pos, neighbor));

      hex->make_road(get_ind_direction(neighbor, pos));
    }
  }

  manager->get_hexagon(pos)->make_road(directions);

  for (auto neighbor : neighbors)
  {
    std::shared_ptr<Hexagon> hex = manager->get_hexagon(neighbor);
    if (hex->frames.find(Road) != hex->frames.end())
    {
      manager->get_rectangle(pos, neighbor)->make_road();
    }
  }
}

void Hexoworld::add_farm_in_hex(uint32_t row, uint32_t col)
{
  Coord pos(row, col);
  std::shared_ptr<Hexagon> hex = manager->get_hexagon(pos);
  std::set<Coord> neighbors = manager->get_neighbors(pos);

  for (auto neighbor_pos : neighbors)
  {
    std::shared_ptr<Hexagon> neighbor = manager->get_hexagon(neighbor_pos);
    if (neighbor->frames.find(Road) != neighbor->frames.end())
    {
      neighbor->make_road(get_ind_direction(neighbor_pos, pos));
      manager->get_rectangle(pos, neighbor_pos)->make_road();
    }
  }

  hex->make_farm();
}

void Hexoworld::update_river()
{
  for (auto& river : manager->get_all_rivers())
  {
    for (int i = river.size() - 1; i > 0; --i)
    {
      if (dynamic_cast<Hexagon*>(river[i].get()))
      {
        auto hex  = std::static_pointer_cast<Hexagon  >(river[  i  ]);
        auto rect = std::static_pointer_cast<Rectangle>(river[i - 1]);

        auto hex_drawer  = std::static_pointer_cast<Hexagon  ::RiverDrawer>(hex ->drawers.at(River));
        auto rect_drawer = std::static_pointer_cast<Rectangle::RiverDrawer>(rect->drawers.at(River));

        hex_drawer->set_new_special_color_river(rect_drawer->special_color_river);
      }
      else
      {
        auto hex  = std::static_pointer_cast<Hexagon  >(river[i - 1]);
        auto rect = std::static_pointer_cast<Rectangle>(river[  i  ]);

        auto hex_drawer  = std::static_pointer_cast<Hexagon  ::RiverDrawer>(hex ->drawers.at(River));
        auto rect_drawer = std::static_pointer_cast<Rectangle::RiverDrawer>(rect->drawers.at(River));

        rect_drawer->set_new_special_color_river(hex_drawer->special_color_river);
      }
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-20, 20);
    auto make_new_component = [&gen, &dis](int32_t component) {
      component += dis(gen);
      return std::min(255, std::max(0, component));
      };

    auto first_hex_drawer = std::static_pointer_cast<Hexagon::RiverDrawer>(river[0]->drawers.at(River));

    first_hex_drawer->set_new_special_color_river(
      Eigen::Vector4i(
        make_new_component(riverColor.x()),
        make_new_component(riverColor.y()),
        make_new_component(riverColor.z()),
        make_new_component(riverColor.w())
      ));
  }
}

void Hexoworld::set_hex_height(uint32_t row, uint32_t col, int32_t height)
{
  Coord pos(row, col);
  std::set<Coord> neighbors = manager->get_neighbors(pos);

  manager->get_hexagon(pos)->set_height(height);

  for (Coord posNeighbor : neighbors)
  {
    manager->get_rectangle(pos, posNeighbor)->update();
  }

  for (Coord posNeighbor : neighbors)
  {
    std::vector<Coord> common_neighbors;

    std::set<Coord> neighbors_neighbor = manager->get_neighbors(posNeighbor);
    std::set_intersection(
      neighbors.begin(), neighbors.end(),
      neighbors_neighbor.begin(), neighbors_neighbor.end(),
      std::back_inserter(common_neighbors));

    for (Coord common_neighbor : common_neighbors)
      manager->get_triangle(pos, posNeighbor, common_neighbor)->update();
  }
}

void Hexoworld::set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color)
{
  auto drawer = std::static_pointer_cast<Hexagon::UsualDrawer>(
    manager->get_hexagon(Coord(row, col))->drawers.at(Usual));
  
  drawer->set_color(color);
}

int32_t Hexoworld::get_hex_height(uint32_t row, uint32_t col) const
{
  auto center = Points::get_instance().get_point(
    manager->get_hexagon(Coord(row, col))->mainData->centerId
  );

  int32_t height = round(
      heightDirection_.dot(center - origin_) / 
      heightStep_
    );

  return height;
}

Eigen::Vector4i Hexoworld::get_hex_color(uint32_t row, uint32_t col) const
{
  return std::static_pointer_cast<Hexagon::UsualDrawer>(
    manager->get_hexagon(Coord(row, col))->drawers[Usual]
  )->get_color();
}

uint32_t Hexoworld::get_n_rows() {
  return n_rows;
}

uint32_t Hexoworld::get_n_cols() {
  return n_cols;
}

void Hexoworld::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
#ifdef SPEED_TEST
  auto start = std::chrono::steady_clock::now();
  std::chrono::nanoseconds t_print_in_trilist;
  std::chrono::nanoseconds t_colorize_points;
  std::chrono::nanoseconds t_print_in_vertices;
#endif // SPEED_TEST

  const auto& objects = manager->get_all_object();
  
  std::vector<PrintingPoint> tmp_Vertices;
  std::vector<uint32_t> tmp_TriList;

  auto func_t = [
    &objects, 
    &tmp_TriList
#ifdef SPEED_TEST2
    ,&t_print_in_trilist
#endif // SPEED_TEST
  ]() -> void
    {
#ifdef SPEED_TEST2
      auto start = std::chrono::steady_clock::now();
#endif // SPEED_TEST

      for (uint32_t i = 0; i < objects.size(); ++i)
        objects[i]->print_in_triList(tmp_TriList);

#ifdef SPEED_TEST2
      auto end = std::chrono::steady_clock::now();
      t_print_in_trilist = end - start;
#endif // SPEED_TEST
    };

  auto func_v = [
    &objects, 
    &tmp_Vertices
#ifdef SPEED_TEST2
      , &t_colorize_points
      , &t_print_in_vertices
#endif // SPEED_TEST
  ]() -> void
    {
#ifdef SPEED_TEST2
      auto start = std::chrono::steady_clock::now();
#endif // SPEED_TEST

      uint32_t n_threads = 3;
      auto func = [&](uint32_t phase)
        {
          for (uint32_t i = phase; i < objects.size(); i += n_threads)
            objects[i]->colorize_points();
        };

      std::vector<std::unique_ptr<std::thread>> threads;
      for (uint32_t i = 0; i < n_threads; ++i)
        threads.push_back(std::make_unique<std::thread>(func, i));
      for (uint32_t i = 0; i < n_threads; ++i)
        threads[i]->join();
      threads.clear();

#ifdef SPEED_TEST2
      auto end = std::chrono::steady_clock::now();
      t_colorize_points = end - start;
      start = std::chrono::steady_clock::now();
#endif // SPEED_TEST
      Points::get_instance().print_in_vertices(tmp_Vertices);

#ifdef SPEED_TEST2
      end = std::chrono::steady_clock::now();
      t_print_in_vertices = end - start;
#endif // SPEED_TEST
    };

  std::thread t(func_t);
  std::thread v(func_v);
  t.join();
  v.join();
  /*func_t();
  func_v();*/

  zip_data(tmp_Vertices, tmp_TriList, Vertices, TriList);

#ifdef SPEED_TEST
  auto end = std::chrono::steady_clock::now();
  static uint64_t sum_all = 0;
  static uint64_t sum_print_in_trilist = 0;
  static uint64_t sum_colorize_points = 0;
  static uint64_t sum_print_in_vertices = 0;

  sum_all              += std::chrono::duration_cast<std::chrono::milliseconds>(
    end - start).count();
  sum_print_in_trilist += std::chrono::duration_cast<std::chrono::milliseconds>(
    t_print_in_trilist).count();
  sum_colorize_points  += std::chrono::duration_cast<std::chrono::milliseconds>(
    t_colorize_points).count();
  sum_print_in_vertices+= std::chrono::duration_cast<std::chrono::milliseconds>(
    t_print_in_vertices).count();

  static double cnt = 0;
  cnt += 1;
  if (cnt == 1)
    std::cout << "all trilist colorize vertices" << std::endl;
  std::cout 
    << sum_all / cnt               << "                               "
    << sum_print_in_trilist / cnt  << "                               "
    << sum_colorize_points / cnt   << "                               "
    << sum_print_in_vertices / cnt << "                               "
    << std::endl;
#endif
}

void Hexoworld::zip_data(
  std::vector<PrintingPoint>& Vertices, 
  std::vector<uint32_t>& TriList, 
  std::vector<PrintingPoint>& new_Vertices, 
  std::vector<uint16_t>& new_TriList) const
{
  new_Vertices.clear();
  new_TriList.clear();
  std::map<PrintingPoint, uint16_t> point_to_newId;
  std::unordered_map<uint32_t, uint16_t> oldId_to_newId;

  for (uint32_t oldId : TriList)
  {
    if (oldId_to_newId.find(oldId) == oldId_to_newId.end())
    {
      if (point_to_newId.find(Vertices[oldId]) != point_to_newId.end())
      {
        oldId_to_newId[oldId] = point_to_newId[Vertices[oldId]];
      }
      else
      {
        uint16_t size = new_Vertices.size();
        new_Vertices.push_back(Vertices[oldId]);
        point_to_newId[Vertices[oldId]] = size;
        oldId_to_newId[oldId] = size;
      }
    }

    new_TriList.push_back(oldId_to_newId[oldId]);
  }
  return;
}
