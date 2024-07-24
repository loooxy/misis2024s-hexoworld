#include "hexoworld.hpp"
#include "hexoworld.hpp"
#include "hexoworld.hpp"
#include "hexoworld.hpp"
#include <hexoworld/hexoworld.hpp>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

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
  nTerracesOnHeightStep_(n_terraces_on_height_step)
{
  if (abs(row_direction.dot(сol_direction)) > PRECISION_DBL_CALC)
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
    std::static_pointer_cast<Rectangle::UsualDrawer>(
      manager->get_rectangle(pos, pos_neighbor)->drawers.at(Usual))->set_color(color);

    std::set<Coord> neighbors_second = manager->get_neighbors(pos_neighbor);
    std::vector<Coord> intersection;
    std::set_intersection(neighbors.begin(), neighbors.end(),
      neighbors_second.begin(), neighbors_second.end(),
      std::back_inserter(intersection));
    for (Coord pos_third : intersection)
      std::static_pointer_cast<Triangle::UsualDrawer>(
        manager->get_triangle(pos, pos_neighbor, pos_third)->drawers.at(Usual))->set_color(color);
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
    std::set<Coord> neighbors = manager->get_neighbors(positions[i]);
    if (neighbors.find(positions[i + 1]) == neighbors.end())
      throw std::invalid_argument("Cell " + std::to_string(i) + " and cell " + std::to_string(i + 1) + 
      " are not neighbors.");
  }

  std::vector<std::shared_ptr<Object>> river;
  for (int i = 0; i < positions.size(); ++i)
  {
    uint32_t before_ind, next_ind;
    if (i > 0)
    {
      before_ind = get_ind_direction(positions[i], positions[i - 1]);
      river.push_back(manager->get_rectangle(positions[i], positions[i - 1]));
      manager->get_rectangle(positions[i], positions[i - 1])->make_river();
    }
    else
      before_ind = -1;
    
    if (i < positions.size() - 1)
      next_ind = get_ind_direction(positions[i], positions[i + 1]);
    else
      next_ind = -1;

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
      manager->get_rectangle(pos, neighbor)->make_road();
    }
  }

  manager->get_hexagon(pos)->make_road(directions);
}

void Hexoworld::update_river()
{
  for (auto& river : manager->get_all_rivers())
  {
    for (int i = river.size() - 1; i > 0; --i)
    {
      if (dynamic_cast<Hexagon*>(river[i].get()))
      {
        std::shared_ptr<Hexagon> hex = std::static_pointer_cast<Hexagon>(river[i]);
        std::shared_ptr<Rectangle> rect = std::static_pointer_cast<Rectangle>(river[i - 1]);
        std::static_pointer_cast<Hexagon::RiverDrawer>(hex->drawers.at(River))->set_new_special_color_river(
          std::static_pointer_cast<Rectangle::RiverDrawer>(rect->drawers.at(River))->special_color_river);
      }
      else
      {
        std::shared_ptr<Hexagon> hex = std::static_pointer_cast<Hexagon>(river[i - 1]);
        std::shared_ptr<Rectangle> rect = std::static_pointer_cast<Rectangle>(river[i]);

        std::static_pointer_cast<Rectangle::RiverDrawer>(rect->drawers.at(River))->set_new_special_color_river(
          std::static_pointer_cast<Hexagon::RiverDrawer>(hex->drawers.at(River))->special_color_river);
      }
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-20, 20);
    auto make_new_component = [&gen, &dis](int32_t component) {
      component += dis(gen);
      return std::min(255, std::max(0, component));
      };

    std::static_pointer_cast<Hexagon::RiverDrawer>(river[0]->drawers.at(River))->special_color_river =
      Eigen::Vector4i(
        make_new_component(riverColor.x()),
        make_new_component(riverColor.y()),
        make_new_component(riverColor.z()),
        255
      );
  }
}

void Hexoworld::set_hex_height(uint32_t row, uint32_t col, int32_t height)
{
  Coord pos(row, col);
  manager->get_hexagon(pos)->set_height(height);
  std::set<Coord> neighbors = manager->get_neighbors(pos);
  for (Coord posNeighbor : neighbors)
    manager->get_rectangle(pos, posNeighbor)->update();
}

void Hexoworld::set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color)
{
  Coord pos(row, col);
  std::static_pointer_cast<Hexagon::UsualDrawer>(
    manager->get_hexagon(pos)->drawers.at(Usual))->set_color(color);

  Eigen::Vector4i a = color;
  std::set<Coord> neighbors = manager->get_neighbors(pos);
  for (Coord pos_neighbor : neighbors)
  {
    Eigen::Vector4i b = std::static_pointer_cast<Hexagon::UsualDrawer>(
      manager->get_hexagon(pos_neighbor)->drawers.at(Usual))->get_color();

    std::static_pointer_cast<Rectangle::UsualDrawer>(
      manager->get_rectangle(pos, pos_neighbor)->drawers.at(Usual))->set_color(a + (b - a) / 2);

    std::set<Coord> neighbors_second = manager->get_neighbors(pos_neighbor);
    std::vector<Coord> intersection;
    std::set_intersection(neighbors.begin(), neighbors.end(),
      neighbors_second.begin(), neighbors_second.end(),
      std::back_inserter(intersection));
    for (Coord pos_third : intersection)
    {
      Eigen::Vector4i c = std::static_pointer_cast<Hexagon::UsualDrawer>(
        manager->get_hexagon(pos_third)->drawers.at(Usual))->get_color();
      
      double len_a = (b - a).norm() + (c - a).norm();
      double len_b = (b - a).norm() + (c - b).norm();
      double len_c = (b - c).norm() + (c - a).norm();
      double max_len = std::max(len_a, std::max(len_b, len_c));

      Eigen::Vector4i dop_color = (max_len == len_a ? a : (max_len == len_b ? b : c));

      Eigen::Matrix4i colors = (Eigen::MatrixXi(4, 4) << 
        a.x(), a.y(), a.z(), a.w(),
        b.x(), b.y(), b.z(), b.w(),
        c.x(), c.y(), c.z(), c.w(),
        dop_color.x(), dop_color.y(), dop_color.z(), dop_color.w()/10).finished();

      std::static_pointer_cast<Triangle::UsualDrawer>(
        manager->get_triangle(pos, pos_neighbor, pos_third)->drawers.at(Usual))->set_color(
      Eigen::Vector4i(
      colors.col(0).mean(),
      colors.col(1).mean(),
      colors.col(2).mean(),
      colors.col(3).mean()
      ));
    }
  }
}

void Hexoworld::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  for (auto& object : manager->get_all_object())
    object->print_in_triList(TriList);
  Points::get_instance().print_in_vertices(Vertices);
}

void Hexoworld::recolor(std::vector<PrintingPoint>& Vertices) const
{
  Points::get_instance().recolor(Vertices);
}
