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


void Hexoworld::printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
  std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
  std::vector<uint16_t>& TriList, const Object* base)
{
  uint32_t id1;
  id1 = Points::get_instance().get_id_point(a.first, base);

  uint32_t id2;
  id2 = Points::get_instance().get_id_point(a.second, base);

  uint32_t id3;
  id3 = Points::get_instance().get_id_point(b.first, base);

  uint32_t id4;
  id4 = Points::get_instance().get_id_point(b.second, base);

  printRect({ id1, id2 }, { id3, id4 }, TriList);
}
void Hexoworld::printRect(
  std::pair<uint32_t, uint32_t> aIds,
  std::pair<uint32_t, uint32_t> bIds,
  std::vector<uint16_t>& TriList)
{
  TriList.push_back(aIds.first);
  TriList.push_back(aIds.second);
  TriList.push_back(bIds.first);

  TriList.push_back(bIds.first);
  TriList.push_back(aIds.second);
  TriList.push_back(aIds.first);

  TriList.push_back(aIds.second);
  TriList.push_back(bIds.first);
  TriList.push_back(bIds.second);

  TriList.push_back(bIds.second);
  TriList.push_back(bIds.first);
  TriList.push_back(aIds.second);
}


void Hexoworld::printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  std::vector<uint16_t>& TriList, const Object* base)
{
  uint32_t id1;
  id1 = Points::get_instance().get_id_point(a, base);

  uint32_t id2;
  id2 = Points::get_instance().get_id_point(b, base);

  uint32_t id3;
  id3 = Points::get_instance().get_id_point(c, base);

  printTri(id1, id2, id3, TriList);
}
void Hexoworld::printTri(
  uint32_t aId, uint32_t bId, uint32_t cId, 
  std::vector<uint16_t>& TriList)
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
    add(Coord(pos.row    , pos.col + 1));
    add(Coord(pos.row - 1, pos.col));
    add(Coord(pos.row - 1, pos.col - 1));
    add(Coord(pos.row    , pos.col - 1));
    add(Coord(pos.row + 1, pos.col - 1));
  }
  else
  {
    add(Coord(pos.row + 1, pos.col + 1));
    add(Coord(pos.row    , pos.col + 1));
    add(Coord(pos.row - 1, pos.col + 1));
    add(Coord(pos.row - 1, pos.col));
    add(Coord(pos.row    , pos.col - 1));
    add(Coord(pos.row + 1, pos.col));
  }

  return answer;
}