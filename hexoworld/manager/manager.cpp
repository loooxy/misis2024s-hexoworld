#include "manager.hpp"
#include <hexoworld/hexoworld.hpp>

std::shared_ptr<Hexoworld::Hexagon> Hexoworld::Manager::add_hexagon(Coord coord) {
  return grid_[coord] = std::make_shared<Hexoworld::Hexagon>(
    world, 
    
    world.origin_ +
    world.colDirection_ *
    ((sqrtf(3) * coord.col + (coord.row % 2) * sqrt(3) / 2) * 1.5*world.size_)
    + world.rowDirection_ *
    (1.5 * 1.5*world.size_ * coord.row),
    
    coord
  );
}

std::shared_ptr<Hexoworld::Rectangle> Hexoworld::Manager::add_rectangle(Coord first, Coord second)
{
  const auto pair = pair_coords(first, second);
  
  auto mainData_hex1 = grid_.at(first)->mainData;
  auto mainData_hex2 = grid_.at(second)->mainData;
    
  uint32_t first_ind_side = get_ind_direction(first, second);
  uint32_t second_ind_side = get_ind_direction(second, first);

  std::vector<Eigen::Vector3d> epi2 = mainData_hex2->extraPoints[second_ind_side];
  std::reverse(epi2.begin(), epi2.end());

  return rectangles[pair] = std::make_shared<Rectangle>(
    world,
    
    mainData_hex1->polygonPoints[first_ind_side], 
    mainData_hex1->polygonPoints[(first_ind_side + 1) % 6],
    mainData_hex2->polygonPoints[(second_ind_side + 1) % 6], 
    mainData_hex2->polygonPoints[second_ind_side],
    
    mainData_hex1->extraPoints[first_ind_side], 
    epi2, 
    
    grid_.at(first), 
    grid_.at(second)
  );
}

std::shared_ptr<Hexoworld::Triangle> Hexoworld::Manager::add_triangle(Coord first, Coord second, Coord third)
{
  const auto tri = tri_coords(first, second, third);
  
  auto get_ind = [this](Coord first, Coord second, Coord third) {
    uint32_t a = get_ind_direction(first, second);
    uint32_t b = get_ind_direction(first, third);
    std::set<uint32_t> st1 = { a, (a + 1) % 6 };
    std::set<uint32_t> st2 = { b, (b + 1) % 6 };
    std::vector<uint32_t> intersection;
    std::set_intersection(st1.begin(), st1.end(), st2.begin(), st2.end(), std::back_inserter(intersection));
    return intersection.front();
    };

  const auto& ipi1 = grid_.at(first)->mainData->polygonPoints;
  const auto& ipi2 = grid_.at(second)->mainData->polygonPoints;
  const auto& ipi3 = grid_.at(third)->mainData->polygonPoints;

  return triangles[tri] = std::make_shared<Triangle>(
    world, 
    
    ipi1[get_ind(first, second, third)],
    ipi2[get_ind(second, third, first)],
    ipi3[get_ind(third, first, second)],
    
    grid_.at(first),
    grid_.at(second),
    grid_.at(third)
  );
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
    return add_hexagon(coord);
  return grid_[coord];
}

std::shared_ptr<Hexoworld::Rectangle> Hexoworld::Manager::get_rectangle(Coord first, Coord second)
{
  const auto pair = pair_coords(first, second);

  if (rectangles.find(pair) == rectangles.end())
    return add_rectangle(first, second);

  return rectangles[pair];
}

std::shared_ptr<Hexoworld::Triangle> Hexoworld::Manager::get_triangle(Coord first, Coord second, Coord third)
{
  const auto tri = tri_coords(first, second, third);
  if (triangles.find(tri) == triangles.end())
    return add_triangle(first, second, third);

  return triangles[tri];
}

void Hexoworld::Manager::add_river(std::vector<std::shared_ptr<Object>> river)
{
  rivers.push_back(river);
}

std::vector<std::vector<std::shared_ptr<Hexoworld::Object>>> Hexoworld::Manager::get_all_rivers() const
{
  return rivers;
}
