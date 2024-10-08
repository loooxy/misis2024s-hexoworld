#include <hexoworld/hexoworld.hpp>

uint32_t Hexoworld::get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2)
{
  if (vertex1 > vertex2)
    std::swap(vertex1, vertex2);

  if (vertex1 == 0 && vertex2 == 5)
    return 5;
  else
    return vertex1;
}

void Hexoworld::set_new_height_to_point(Eigen::Vector3d& point, int32_t height,
  FrameAndDrawersTypes type)
{
  int32_t last_height =
    round(heightDirection_
      .dot(point - dop_height(type) - origin_) /
      heightStep_);

  point += (height - last_height) *
    heightStep_ *
    heightDirection_ + dop_height(type);
}

Eigen::Vector3d Hexoworld::set_new_height_to_point(uint32_t id_point, int32_t height, FrameAndDrawersTypes type)
{
  auto point = Points::get_instance().get_point(id_point);
  set_new_height_to_point(point, height, type);
  Points::get_instance().update_point(id_point, point);
  return point;
}


void Hexoworld::printRect(
  std::pair<uint32_t, uint32_t> aIds,
  std::pair<uint32_t, uint32_t> bIds,
  std::vector<uint32_t>& TriList)
{
  printTri(aIds.first, aIds.second, bIds.second, TriList);
  printTri(aIds.first, bIds.first, bIds.second, TriList);
}

void Hexoworld::printTri(
  uint32_t aId, uint32_t bId, uint32_t cId, 
  std::vector<uint32_t>& TriList)
{
  TriList.push_back(aId);
  TriList.push_back(bId);
  TriList.push_back(cId);

  TriList.push_back(cId);
  TriList.push_back(bId);
  TriList.push_back(aId);
}

uint32_t Hexoworld::get_ind_direction(Coord a, Coord b) {
  int32_t row_diff = b.row - a.row;
  int32_t col_diff = b.col - a.col;

  if (row_diff == 0)
    return (col_diff == 1 ? 1 : 4);

  if (a.row % 2 == 0)
  {
    if (row_diff == 1)
      return (col_diff == 0 ? 0 : 5);
    if (row_diff == -1)
      return (col_diff == 0 ? 2 : 3);
  }
  else
  {
    if (row_diff == 1)
      return (col_diff == 0 ? 5 : 0);
    if (row_diff == -1)
      return (col_diff == 0 ? 3 : 2);
  }
}

std::pair<Hexoworld::Coord, Hexoworld::Coord>
  Hexoworld::pair_coords(Coord a, Coord b)
{
  if (b < a)
    std::swap(a, b);
  return std::pair<Coord, Coord>(a, b);
};

std::vector<Hexoworld::Coord> 
Hexoworld::tri_coords(Coord a, Coord b, Coord c)
{
  std::vector<Coord> answer = { a, b, c };
  std::sort(answer.begin(), answer.end());
  return answer;
}

