#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <algorithm>

uint32_t Hexoworld::HexagonGrid::connect(Eigen::Vector3d point, std::shared_ptr<HexagonGrid::Object> object)
{
  return HexagonGrid::Points::get_instance().connect_point_with_object(point, object);
}
void Hexoworld::HexagonGrid::printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
  std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList)
{
  uint32_t id1;
  if (Points::get_instance().in_points(a.first))
    id1 = Points::get_instance().get_id_point(a.first);
  else
  {
    id1 = Vertices.size();
    Vertices.push_back(a.first);
  }

  uint32_t id2;
  if (Points::get_instance().in_points(a.second))
    id2 = Points::get_instance().get_id_point(a.second);
  else
  {
    id2 = Vertices.size();
    Vertices.push_back(a.second);
  }

  uint32_t id3;
  if (Points::get_instance().in_points(b.first))
    id3 = Points::get_instance().get_id_point(b.first);
  else
  {
    id3 = Vertices.size();
    Vertices.push_back(b.first);
  }
  uint32_t id4;
  if (Points::get_instance().in_points(b.second))
    id4 = Points::get_instance().get_id_point(b.second);
  else
  {
    id4 = Vertices.size();
    Vertices.push_back(b.second);
  }

  TriList.push_back(id1);
  TriList.push_back(id2);
  TriList.push_back(id3);

  TriList.push_back(id3);
  TriList.push_back(id2);
  TriList.push_back(id1);

  TriList.push_back(id2);
  TriList.push_back(id3);
  TriList.push_back(id4);

  TriList.push_back(id4);
  TriList.push_back(id3);
  TriList.push_back(id2);
}
void Hexoworld::HexagonGrid::printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList)
{
  uint32_t id1;
  if (Points::get_instance().in_points(a))
    id1 = Points::get_instance().get_id_point(a);
  else
  {
    id1 = Vertices.size();
    Vertices.push_back(a);
  }

  uint32_t id2;
  if (Points::get_instance().in_points(b))
    id2 = Points::get_instance().get_id_point(b);
  else
  {
    id2 = Vertices.size();
    Vertices.push_back(b);
  }

  uint32_t id3;
  if (Points::get_instance().in_points(c))
    id3 = Points::get_instance().get_id_point(c);
  else
  {
    id3 = Vertices.size();
    Vertices.push_back(c);
  }

  TriList.push_back(id1);
  TriList.push_back(id2);
  TriList.push_back(id3);

  TriList.push_back(id3);
  TriList.push_back(id2);
  TriList.push_back(id1);
}

//struct PrintingPoint-------------------------------------

PrintingPoint::PrintingPoint(Eigen::Vector3d position)
  : x(position.x()), y(position.y()), z(position.z()) {}

//class Points---------------------------------------------
uint32_t Hexoworld::HexagonGrid::Points::connect_point_with_object(Eigen::Vector3d p,
  std::shared_ptr<Object> object)
{
  uint32_t id;
  if (point_to_id.find(p) == point_to_id.end())
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_objects.push_back(std::vector<std::shared_ptr<Object>>());
    id_to_objects.back().push_back(object);
  }
  else
  {
    id = point_to_id[p];
    id_to_objects[id].push_back(object);
  }
  return id;
}

bool Hexoworld::HexagonGrid::Points::in_points(Eigen::Vector3d p) const
{
  return (point_to_id.find(p) != point_to_id.end());
}

uint32_t Hexoworld::HexagonGrid::Points::get_id_point(Eigen::Vector3d p)
{
  uint32_t id;
  if (point_to_id.find(p) == point_to_id.end())
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_objects.push_back(std::vector<std::shared_ptr<Object>>());
  }
  else
  {
    id = point_to_id[p];
  }
  return id;
}

std::vector<std::shared_ptr<Hexoworld::HexagonGrid::Object>> 
Hexoworld::HexagonGrid::Points::get_objects(uint32_t id)
{
  if (id >= id_to_objects.size())
    throw std::invalid_argument("wrong id");
  return id_to_objects[id];
}

Eigen::Vector3d 
Hexoworld::HexagonGrid::Points::get_point(uint32_t id) const
{
  if (id >= id_to_point.size())
    throw std::invalid_argument("wrong id");
  return id_to_point[id];
}

void Hexoworld::HexagonGrid::Points::update_point(uint32_t id, Eigen::Vector3d new_point)
{
  Eigen::Vector3d point = Points::get_instance().id_to_point[id];
  Points::get_instance().point_to_id.erase(point);
  Points::get_instance().point_to_id[new_point] = id;
  Points::get_instance().id_to_point[id] = new_point;
}

//struct Hexagon-------------------------------------------

Hexoworld::HexagonGrid::Hexagon::Hexagon(Hexoworld& hexoworld,
  float big_size, float small_size,
  Eigen::Vector3d center,
  Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection)
  : center(center), world(hexoworld) {
  if (abs(pointDirection.dot(floatDirection)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");

  pointDirection.normalize();
  floatDirection.normalize();
  outerPoints.resize(6);
  {
    outerPoints[0] = center + (pointDirection * big_size);
    outerPoints[1] =
      center + (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[2] =
      center - (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[3] = center - (pointDirection * big_size);
    outerPoints[4] =
      center - (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[5] =
      center + (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size));
  }
  innerPoints.resize(6);
  {
    innerPoints[0] = center + (pointDirection * small_size);
    innerPoints[1] =
      center + (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[2] =
      center - (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[3] = center - (pointDirection * small_size);
    innerPoints[4] =
      center - (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[5] =
      center + (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size));
  }
}
void Hexoworld::HexagonGrid::Hexagon::connect_points(std::shared_ptr<Hexagon> ptr)
{
  innerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    innerPointsId[i] = connect(innerPoints[i], ptr);
  outerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    outerPointsId[i] = connect(outerPoints[i], ptr);
}

std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::Hexagon::get_points()
{
  std::vector<Eigen::Vector3d> answer;
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d p = Points::get_instance().get_point(innerPointsId[i]);
    answer.push_back(p);
  }
  answer.push_back(center);
  return answer;
}
void Hexoworld::HexagonGrid::Hexagon::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  int ind = Vertices.size();
  Vertices.push_back(center);
  for (int i = 0; i < 6; ++i)
  {
    TriList.push_back(ind);
    TriList.push_back(innerPointsId[i]);
    TriList.push_back(innerPointsId[(i + 1) % 6]);

    TriList.push_back(innerPointsId[(i + 1) % 6]);
    TriList.push_back(innerPointsId[i]);
    TriList.push_back(ind);
  }
}

//struct Triangle------------------------------------------

Hexoworld::HexagonGrid::Triangle::Triangle(Hexoworld& hexoworld, 
  Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c)
: world(hexoworld) {
  std::vector<uint32_t> ids = {
    Points::get_instance().get_id_point(a) ,
    Points::get_instance().get_id_point(b) ,
    Points::get_instance().get_id_point(c) };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

Hexoworld::HexagonGrid::Triangle::Triangle(Hexoworld& hexoworld,
  uint32_t aId, uint32_t bId, uint32_t cId)
  : world(hexoworld) {
  std::vector<uint32_t> ids = { aId , bId , cId };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

bool Hexoworld::HexagonGrid::Triangle::operator<(const Triangle& rhs) const
{
  if (AId < rhs.AId)
    return true;
  if (AId > rhs.AId)
    return false;

  if (BId < rhs.BId)
    return true;
  if (BId > rhs.BId)
    return false;

  if (CId < rhs.CId)
    return true;
  return false;
}

void Hexoworld::HexagonGrid::Triangle::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList) const {
  Eigen::Vector3d a = Points::get_instance().get_point(AId);
  Eigen::Vector3d b = Points::get_instance().get_point(BId);
  Eigen::Vector3d c = Points::get_instance().get_point(CId);

  int32_t heightA =
    round(world.hexagonGrid_.heightDirection_.dot(
      a - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);
  int32_t heightB =
    round(world.hexagonGrid_.heightDirection_.dot(
      b - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);
  int32_t heightC =
    round(world.hexagonGrid_.heightDirection_.dot(
      c - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);

  std::vector<std::pair<int32_t, Eigen::Vector3d>> points;
  points.push_back({ heightA, a });
  points.push_back({ heightB, b });
  points.push_back({ heightC, c });
  std::sort(points.begin(), points.end(), 
    [](const std::pair<int32_t, Eigen::Vector3d>& a,
      const std::pair<int32_t, Eigen::Vector3d>& b)
    {
      if (a.first != b.first)
        return a.first < b.first;
      else
      {
        EigenVector3dComp cmp;
        return cmp(a.second, b.second);
      }
    }
  );

  if (points[0].first == points[2].first)
  {
    TriList.push_back(AId);
    TriList.push_back(BId);
    TriList.push_back(CId);

    TriList.push_back(CId);
    TriList.push_back(BId);
    TriList.push_back(AId);
  }
  else
  {
    if (points[0].first != points[1].first &&
      points[1].first != points[2].first)
    {
      uint32_t nTerraces =
        (points[2].first - points[0].first) *
        (world.hexagonGrid_.nTerracesOnHeightStep_ + 1) - 1;
      Eigen::Vector3d step =
        (points[2].second -
          points[0].second) / (nTerraces + 1);
      Eigen::Vector3d platformVector =
        world.hexagonGrid_.heightDirection_
        .cross(step)
        .cross(world.hexagonGrid_.heightDirection_)
        .normalized();
      platformVector *= step.dot(platformVector) / 3;

      Eigen::Vector3d goal = points[0].second +
        step * (world.hexagonGrid_.nTerracesOnHeightStep_ + 1)
        * (points[1].first - points[0].first);

      double len = (points[0].second
        - points[2].second).norm();

      Eigen::Vector3d p1;
      p1 = goal - platformVector;

      Eigen::Vector3d p2;
      p2 = goal + platformVector;


      print_stair(points[1].second, p1, points[0].second,
        points[1].second, goal, points[0].second,
        Vertices, TriList);
      print_stair(points[1].second, p2, points[2].second,
        points[1].second, goal, points[2].second,
        Vertices, TriList);

      printTri(points[1].second, p1, p2,
        Vertices, TriList);
    }
    else
    {
      if (points[0].first == points[1].first)
        print_stair(points[0].second,
          points[1].second,
          points[2].second,
          points[0].second,
          points[1].second,
          points[2].second,
          Vertices, TriList);
      else
        print_stair(points[1].second,
          points[2].second,
          points[0].second,
          points[1].second,
          points[2].second,
          points[0].second,
          Vertices, TriList);
    }
  }
}

void Hexoworld::HexagonGrid::Triangle::print_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  Eigen::Vector3d a_goal,
  Eigen::Vector3d b_goal,
  Eigen::Vector3d c_goal,
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList) const
{
  Eigen::Vector3d d = c;
  Eigen::Vector3d d_goal = c_goal;
  int32_t heightstart =
    round(world.hexagonGrid_.heightDirection_.dot(
      a_goal - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);
  int32_t heightend =
    round(world.hexagonGrid_.heightDirection_.dot(
      c_goal - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(b, d);
    std::swap(a_goal, c_goal);
    std::swap(b_goal, d_goal);
    std::swap(heightstart, heightend);
  }

  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> stairs;
  stairs.push_back({ a, b });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      (heightend - heightstart) *
      (world.hexagonGrid_.nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c_goal - a_goal) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      world.hexagonGrid_.heightDirection_
      .cross(step1)
      .cross(world.hexagonGrid_.heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;

    Eigen::Vector3d step2 =
      (d_goal - b_goal) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      world.hexagonGrid_.heightDirection_
      .cross(step2)
      .cross(world.hexagonGrid_.heightDirection_)
      .normalized();
    platformVector2 *= step2.dot(platformVector2) / 3;
    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a - c).norm();
      Eigen::Vector3d p1, p2, p3, p4;

      p1 = a_goal + step1 * i - platformVector1;
      p2 = b_goal + step2 * i - platformVector2;
      p3 = a_goal + step1 * i + platformVector1;
      p4 = b_goal + step2 * i + platformVector2;

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], Vertices, TriList);
}

//struct Rectangle------------------------------------------

Hexoworld::HexagonGrid::BorderRectangle::BorderRectangle(
  Hexoworld& hexoworld,
  Eigen::Vector3d a, Eigen::Vector3d b,
  Eigen::Vector3d c, Eigen::Vector3d d) 
: world(hexoworld) {
  AId = Points::get_instance().get_id_point(a);
  BId = Points::get_instance().get_id_point(b);
  CId = Points::get_instance().get_id_point(c);
  DId = Points::get_instance().get_id_point(d);
}

Hexoworld::HexagonGrid::BorderRectangle::BorderRectangle(
  Hexoworld& hexoworld,
  uint32_t aId, uint32_t bId,
  uint32_t cId, uint32_t dId)
  : world(hexoworld) {
  AId = aId;
  BId = bId;
  CId = cId;
  DId = dId;
}

bool Hexoworld::HexagonGrid::BorderRectangle::operator<(const BorderRectangle& rhs) const
{
  std::vector<uint32_t> ids_lhs = { AId , BId , CId, DId };
  std::vector<uint32_t> ids_rhs = { rhs.AId , rhs.BId ,
    rhs.CId, rhs.DId };

  std::sort(ids_lhs.begin(), ids_lhs.end());
  std::sort(ids_rhs.begin(), ids_rhs.end());

  for (int i = 0; i < 4; ++i)
  {
    if (ids_lhs[i] < ids_rhs[i])
      return true;
    if (ids_lhs[i] > ids_rhs[i])
      return false;
  }
  return false;
}

void Hexoworld::HexagonGrid::BorderRectangle::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList) const {
  Eigen::Vector3d a = Points::get_instance().get_point(AId);
  Eigen::Vector3d b = Points::get_instance().get_point(BId);
  Eigen::Vector3d c = Points::get_instance().get_point(CId);
  Eigen::Vector3d d = Points::get_instance().get_point(DId);

  int32_t heightstart =
    round(world.hexagonGrid_.heightDirection_.dot(
      a - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);
  int32_t heightend =
    round(world.hexagonGrid_.heightDirection_.dot(
      c - world.hexagonGrid_.origin_) /
      world.hexagonGrid_.heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(b, d);
    std::swap(heightstart, heightend);
  }

  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> stairs;
  stairs.push_back({ a, b });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      (heightend - heightstart) *
      (world.hexagonGrid_.nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c - a) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      world.hexagonGrid_.heightDirection_
      .cross(step1)
      .cross(world.hexagonGrid_.heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;
    Eigen::Vector3d step2 = (d - b) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      world.hexagonGrid_.heightDirection_
      .cross(step2)
      .cross(world.hexagonGrid_.heightDirection_)
      .normalized();
    platformVector2 *= step2.dot(platformVector2) / 3;
    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a - c).norm();
      Eigen::Vector3d p1 = a + step1 * i - platformVector1;
      Eigen::Vector3d p2 = b + step2 * i - platformVector2;
      Eigen::Vector3d p3 = a + step1 * i + platformVector1;
      Eigen::Vector3d p4 = b + step2 * i + platformVector2;

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], Vertices, TriList);
}

//struct HexagonGrid---------------------------------------
Hexoworld::HexagonGrid::HexagonGrid(
  Hexoworld& hexoworld, 
  float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, Eigen::Vector3d  col_direction,
  float height_step, uint32_t n_terraces_on_height_step)
  : origin_(origin), size_(size), innerSize_(0.75 * size),
  heightStep_(height_step),
  nTerracesOnHeightStep_(n_terraces_on_height_step),
  world(hexoworld)
{
  if (abs(col_direction.dot(row_direction)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("�ol_direction and row_direction not perpendicular");

  row_direction.normalize();
  col_direction.normalize();

  rowDirection_ = row_direction;
  colDirection_ = col_direction;
  heightDirection_ = rowDirection_.cross(colDirection_);
}

void Hexoworld::HexagonGrid::add_hexagon(uint32_t row, uint32_t col)
{
  if (grid_.find(Coord(row, col)) != grid_.end())
    return;

  std::shared_ptr<Hexagon> ptr =
    std::make_shared<Hexagon>(world, size_, innerSize_,
      origin_ +
      colDirection_ *
      ((sqrtf(3) * col + (row % 2) * sqrt(3) / 2) * size_)
      + rowDirection_ *
      (1.5 * size_ * row),
      rowDirection_, colDirection_);
  grid_[Coord(row, col)] = ptr;
  ptr->connect_points(ptr);

  for (int vertex = 0; vertex < 6; ++vertex)
  {
    uint32_t id = ptr->outerPointsId[vertex];
    std::vector<std::shared_ptr<Object>> objs =
      Points::get_instance().get_objects(id);
    std::vector<std::shared_ptr<Hexagon>> hexs;
    for (const auto& obj : objs)
      if (obj->is_hexagon())
        hexs.push_back(std::static_pointer_cast<Hexagon>(obj));

    //init triangles-------------------------------------
    std::vector<uint32_t> idInnerPoints;
    for (const auto& hex : hexs)
      for (int j = 0; j < 6; ++j)
        if (hex->outerPointsId[j] == id)
          idInnerPoints.push_back(hex->innerPointsId[j]);

    if (idInnerPoints.size() == 3)
    {
      triangles.insert(Triangle(world,
        idInnerPoints[0], idInnerPoints[1], idInnerPoints[2]));
    }

    //init rectangles-------------------------------------
    for (int i_hex1 = 0; i_hex1 < hexs.size(); ++i_hex1)
      for (int i_hex2 = i_hex1 + 1; i_hex2 < hexs.size(); ++i_hex2)
      {
        const auto& hex1 = hexs[i_hex1];
        const auto& hex2 = hexs[i_hex2];
        std::vector<int> hex1_inds_common_vertex;
        std::vector<int> hex2_inds_common_vertex;
        for (int vertex1 = 0; vertex1 < 6; vertex1++)
          for (int vertex2 = 0; vertex2 < 6; vertex2++)
          {
            if (hex1->outerPointsId[vertex1] ==
              hex2->outerPointsId[vertex2])
            {
              hex1_inds_common_vertex.push_back(
                hex1->innerPointsId[vertex1]);
              hex2_inds_common_vertex.push_back(
                hex2->innerPointsId[vertex2]);
            }
          }
        rectangles.insert(BorderRectangle(world,
          hex1_inds_common_vertex[0],
          hex1_inds_common_vertex[1],
          hex2_inds_common_vertex[0],
          hex2_inds_common_vertex[1]
        ));
      }
  }
}

void Hexoworld::HexagonGrid::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  Points::get_instance().print_in_vertices(Vertices);
  for (const auto& [coord, hex] : grid_)
    hex->print_in_vertices_and_triList(Vertices, TriList);
  for (const auto& triangle : triangles)
    triangle.print_in_vertices_and_triList(Vertices, TriList);
  for (const auto& rectangle : rectangles)
    rectangle.print_in_vertices_and_triList(Vertices, TriList);
}
void Hexoworld::HexagonGrid::set_height(int row, int col, int32_t height)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  Eigen::Vector3d& center = grid_.at(Coord(row, col))->center;
  int32_t last_height =
    round(heightDirection_.dot(center - origin_) /
      heightStep_);
  center += (height - last_height) * heightStep_ * heightDirection_;
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_.at(Coord(row, col))->innerPointsId[i];

    Eigen::Vector3d p = Points::get_instance().get_point(id);
    last_height =
      round(heightDirection_.dot(p - origin_) /
        heightStep_);
    p += (height - last_height) * heightStep_ * heightDirection_;

    Points::get_instance().update_point(id, p);
  }
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_.at(Coord(row, col))->outerPointsId[i];
    Eigen::Vector3d p = Points::get_instance().get_point(id);

    Eigen::Matrix3d positions;
    int cnt = 0;
    for (const auto obj : Points::get_instance().get_objects(id))
      if (obj->is_hexagon())
      {
        const Hexagon& hex = *std::static_pointer_cast<Hexagon>(obj);
        positions(cnt, 0) = hex.center.x();
        positions(cnt, 1) = hex.center.y();
        positions(cnt, 2) = hex.center.z();
        cnt++;
      }

    p = Eigen::Vector3d(
      positions.col(0).mean(),
      positions.col(1).mean(),
      positions.col(2).mean());

    Points::get_instance().update_point(id, p);
  }
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::get_hex_points(int row, int col)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  std::vector<Eigen::Vector3d> ans = grid_.at(Coord(row, col))->get_points();
  return ans;
}
