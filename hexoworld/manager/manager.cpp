#include "manager.hpp"
#include "manager.hpp"
#include "manager.hpp"
#include <hexoworld/manager/manager.hpp>
#include <hexoworld/base_objects/hexagon/hexagon.hpp>
#include <hexoworld/base_objects/rectangle/rectangle.hpp>
#include <hexoworld/base_objects/triangle/triangle.hpp>

void Hexoworld::Manager::add_hexagon(Coord coord) {
  if (grid_.find(coord) == grid_.end())
  {
    grid_[coord] = std::make_shared<Hexoworld::Hexagon>(
      world,

      world.origin_ +
      world.colDirection_ *
      ((sqrtf(3) * coord.col + (coord.row % 2) * sqrt(3) / 2) * 1.5 * world.size_)
      + world.rowDirection_ *
      (1.5 * 1.5 * world.size_ * coord.row),

      coord
    );
  }
}

void Hexoworld::Manager::del_hexagon(Coord coord)
{
  if (grid_.find(coord) != grid_.end())
    grid_.erase(coord);
}

void Hexoworld::Manager::add_rectangle(Coord first, Coord second)
{
  const auto pair = pair_coords(first, second);
  if (rectangles.find(pair) == rectangles.end())
  {
    auto mainData_hex1 = grid_.at(first)->mainData;
    auto mainData_hex2 = grid_.at(second)->mainData;

    uint32_t first_ind_side = get_ind_direction(first, second);
    uint32_t second_ind_side = get_ind_direction(second, first);

    std::vector<IdType> epi1Id = mainData_hex1->extraPointsId[first_ind_side];
    std::vector<IdType> epi2Id = mainData_hex2->extraPointsId[second_ind_side];
    std::reverse(epi2Id.begin(), epi2Id.end());
    
    std::vector<Eigen::Vector3d> epi1;
    for (IdType i : epi1Id)
      epi1.push_back(Points::get_instance().get_point(i));
    std::vector<Eigen::Vector3d> epi2;
    for (IdType i : epi2Id)
      epi2.push_back(Points::get_instance().get_point(i));

    rectangles[pair] = std::make_shared<Rectangle>(
      world,

      Points::get_instance().get_point(mainData_hex1->polygonPointsId[first_ind_side]),
      Points::get_instance().get_point(mainData_hex1->polygonPointsId[(first_ind_side + 1) % 6]),
      Points::get_instance().get_point(mainData_hex2->polygonPointsId[(second_ind_side + 1) % 6]),
      Points::get_instance().get_point(mainData_hex2->polygonPointsId[second_ind_side]),

      epi1,
      epi2,

      grid_.at(first),
      grid_.at(second)
    );
  }
}

void Hexoworld::Manager::del_rectangle(Coord first, Coord second)
{
  auto pair = pair_coords(first, second);
  if (rectangles.find(pair) != rectangles.end())
    rectangles.erase(pair);
}

void Hexoworld::Manager::add_triangle(Coord first, Coord second, Coord third)
{
  const auto tri = tri_coords(first, second, third);
  if (triangles.find(tri) == triangles.end())
  { 

    auto get_ind = [this](Coord first, Coord second, Coord third) {
      uint32_t a = get_ind_direction(first, second);
      uint32_t b = get_ind_direction(first, third);
      std::set<uint32_t> st1 = { a, (a + 1) % 6 };
      std::set<uint32_t> st2 = { b, (b + 1) % 6 };
      std::vector<uint32_t> intersection;
      std::set_intersection(st1.begin(), st1.end(), st2.begin(), st2.end(), std::back_inserter(intersection));
      return intersection.front();
      };

    const auto& pp1 = grid_.at(first)->mainData->polygonPointsId;
    const auto& pp2 = grid_.at(second)->mainData->polygonPointsId;
    const auto& pp3 = grid_.at(third)->mainData->polygonPointsId;

    triangles[tri] = std::make_shared<Triangle>(
      world,

      Points::get_instance().get_point(pp1[get_ind(first, second, third)]),
      Points::get_instance().get_point(pp2[get_ind(second, third, first)]),
      Points::get_instance().get_point(pp3[get_ind(third, first, second)]),

      grid_.at(first),
      grid_.at(second),
      grid_.at(third)
    );
  }
}

void Hexoworld::Manager::del_triangle(Coord first, Coord second, Coord third)
{
  auto tri = tri_coords(first, second, third);
  if (triangles.find(tri) != triangles.end())
    triangles.erase(tri);
}

std::vector<std::shared_ptr<Hexoworld::Object>> Hexoworld::Manager::get_all_object()
{
  std::vector<std::shared_ptr<Object>> answer;
  for (const auto& [coord, object] : grid_)
    answer.push_back(object);
  for (const auto& [coord, object] : rectangles)
    answer.push_back(object);
  for (const auto& [coord, object] : triangles)
    answer.push_back(object);
  return answer;
}

std::shared_ptr<Hexoworld::Hexagon> Hexoworld::Manager::get_hexagon(Coord coord)
{
  if (grid_.find(coord) == grid_.end())
    add_hexagon(coord);

  return grid_.at(coord);
}

std::shared_ptr<Hexoworld::Rectangle> Hexoworld::Manager::get_rectangle(Coord first, Coord second)
{
  const auto pair = pair_coords(first, second);

  if (rectangles.find(pair) == rectangles.end())
    add_rectangle(first, second);

  return rectangles.at(pair);
}

std::shared_ptr<Hexoworld::Triangle> Hexoworld::Manager::get_triangle(Coord first, Coord second, Coord third)
{
  const auto tri = tri_coords(first, second, third);
  if (triangles.find(tri) == triangles.end())
    add_triangle(first, second, third);

  return triangles.at(tri);
}

void Hexoworld::Manager::add_river(std::vector<std::shared_ptr<Object>> river)
{
  rivers.push_back(river);
}

std::vector<std::vector<std::shared_ptr<Hexoworld::Object>>> Hexoworld::Manager::get_all_rivers() const
{
  return rivers;
}

std::set<Hexoworld::Coord> Hexoworld::Manager::get_neighbors(Hexoworld::Coord pos)
{
  std::set<Coord> answer;
  const auto add = [&answer, this](Coord pos) {
    if (grid_.find(pos) != grid_.end())
      answer.insert(pos);
    };
  if (pos.row % 2 == 0)
  {
    add(Coord(pos.row + 1, pos.col));
    add(Coord(pos.row, pos.col + 1));
    add(Coord(pos.row - 1, pos.col));
    add(Coord(pos.row - 1, pos.col - 1));
    add(Coord(pos.row, pos.col - 1));
    add(Coord(pos.row + 1, pos.col - 1));
  }
  else
  {
    add(Coord(pos.row + 1, pos.col + 1));
    add(Coord(pos.row, pos.col + 1));
    add(Coord(pos.row - 1, pos.col + 1));
    add(Coord(pos.row - 1, pos.col));
    add(Coord(pos.row, pos.col - 1));
    add(Coord(pos.row + 1, pos.col));
  }

  return answer;
}