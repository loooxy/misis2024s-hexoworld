#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

using namespace Hexoworld;

Color::Color() : Color(0, 0, 0) {}
Color::Color(uint8_t red, uint8_t blue,
  uint8_t green, uint8_t alpha, uint32_t n_parts) 
  : n_parts_(n_parts) {
  abgr_ = (uint32_t(red) << 0) |
    (uint32_t(blue) << 8) |
    (uint32_t(green) << 16) |
    (uint32_t(alpha) << 24);
}
Color::Color(uint32_t abgr, uint32_t n_parts)
  : abgr_(abgr), n_parts_(n_parts) {}
Color Color::operator+(const Color& rhs) const
{
  uint32_t a =
    (uint8_t(abgr_ >> 24) * n_parts_ + 
      uint8_t(rhs.abgr_ >> 24) * rhs.n_parts_) / 
    (n_parts_ + rhs.n_parts_);
  uint32_t b =
    (uint8_t(abgr_ >> 16) * n_parts_ +
      uint8_t(rhs.abgr_ >> 16) * rhs.n_parts_) /
    (n_parts_ + rhs.n_parts_);
  uint32_t g =
    (uint8_t(abgr_ >> 8) * n_parts_ +
      uint8_t(rhs.abgr_ >> 8) * rhs.n_parts_) /
    (n_parts_ + rhs.n_parts_);
  uint32_t r =
    (uint8_t(abgr_) * n_parts_ +
      uint8_t(rhs.abgr_) * rhs.n_parts_) /
    (n_parts_ + rhs.n_parts_);
  return Color(r, g, b, a, n_parts_ + rhs.n_parts_);
}
Color Color::operator-(const Color& rhs) const
{
  if (n_parts_ == rhs.n_parts_)
    if (true/*abgr_ == rhs.abgr_*/)
      return Color(0, 0, 0);
    else
      throw std::invalid_argument("wrong parameter");
    
  uint32_t a =
    (uint8_t(abgr_ >> 24) * n_parts_ - 
      uint8_t(rhs.abgr_ >> 24) * rhs.n_parts_) / 
    (n_parts_ - rhs.n_parts_);
  uint32_t b =
    (uint8_t(abgr_ >> 16) * n_parts_ -
      uint8_t(rhs.abgr_ >> 16) * rhs.n_parts_) /
    (n_parts_ - rhs.n_parts_);
  uint32_t g =
    (uint8_t(abgr_ >> 8) * n_parts_ -
      uint8_t(rhs.abgr_ >> 8) * rhs.n_parts_) /
    (n_parts_ - rhs.n_parts_);
  uint32_t r =
    (uint8_t(abgr_) * n_parts_ -
      uint8_t(rhs.abgr_) * rhs.n_parts_) /
    (n_parts_ - rhs.n_parts_);

  if (r < 0 || g < 0 || b < 0 || a < 0)
    throw std::invalid_argument("wrong parameter");
  return Color(r, g, b, a, n_parts_ - rhs.n_parts_);
}
uint32_t Color::get_abgr() const
{
  return abgr_;
}

uint32_t HexagonGrid::connect(Point point, std::shared_ptr<HexagonGrid::Object> object)
{
  return HexagonGrid::Points::get_instance().connect_point_with_object(point, object);
}
void HexagonGrid::printRect(std::pair<Point, Point> a,
  std::pair<Point, Point> b,
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
void HexagonGrid::printTri(Point a, Point b, Point c,
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

//struct Point---------------------------------------------
Point::Point() : position(0, 0, 0), color(0, 0, 0) {};
Point::Point(Eigen::Vector3d pos, 
  Color color)
  : position(pos), color(color) {}
Point::Point(double x, double y, double z, Color color)
 : Point(Eigen::Vector3d(x, y, z), color) {}
Point Point::operator+(Eigen::Vector3d v)
{
  return Point(position + v, color);
}
Point Point::operator-(Eigen::Vector3d v)
{
  return Point(position  - v, color);
}
Point& Point::operator+=(Eigen::Vector3d v)
{
  position += v;
  return *this;
}
bool Point::operator<(const Point& rhs) const
{
  double min_dif = PRECISION_DBL_CALC;
  if (position.x() + min_dif < rhs.position.x())
    return true;
  else if (position.x() - min_dif > rhs.position.x())
    return false;

  if (position.y() + min_dif < rhs.position.y())
    return true;
  else if (position.y() - min_dif > rhs.position.y())
    return false;

  if (position.z() + min_dif < rhs.position.z())
    return true;
  else
    return false;
}
//struct PrintingPoint-------------------------------------
PrintingPoint::PrintingPoint(Point p)
  : x(p.position.x()), 
  y(p.position.y()), 
  z(p.position.z()), 
  abgr(p.color.get_abgr()) {}
//class Points---------------------------------------------
uint32_t HexagonGrid::Points::connect_point_with_object(Point p,
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
    id_to_point[id].color = id_to_point[id].color + p.color;
    id_to_objects[id].push_back(object);
  }
  return id;
}

bool HexagonGrid::Points::in_points(Point p) const
{
  return (point_to_id.find(p) != point_to_id.end());
}
uint32_t HexagonGrid::Points::get_id_point(Point p)
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
std::vector<std::shared_ptr<HexagonGrid::Object>> HexagonGrid::Points::get_objects(uint32_t id)
{
  if (id >= id_to_objects.size())
    throw std::invalid_argument("wrong id");
  return id_to_objects[id];
}
Point HexagonGrid::Points::get_point(uint32_t id) const
{
  if (id >= id_to_point.size())
    throw std::invalid_argument("wrong id");
  return id_to_point[id];
}
void HexagonGrid::Points::update_point(uint32_t id, Point new_point)
{
  Point point = Points::get_instance().id_to_point[id];
  Points::get_instance().point_to_id.erase(point);
  Points::get_instance().point_to_id[new_point] = id;
  Points::get_instance().id_to_point[id] = new_point;
}

//struct Hexagon-------------------------------------------

HexagonGrid::Hexagon::Hexagon(float big_size, float small_size,
  Eigen::Vector3d center,
  Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection,
  Color color) : center(center){
  if (abs(pointDirection.dot(floatDirection)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");
  
  pointDirection.normalize();
  floatDirection.normalize();
  outerPoints.resize(6);
  {
    outerPoints[0] = Point(center + (pointDirection * big_size),
      color);
    outerPoints[1] = Point(
      center + (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size)),
      color);
    outerPoints[2] = Point(
      center - (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size)),
      color);
    outerPoints[3] = Point(center - (pointDirection * big_size),
      color);
    outerPoints[4] = Point(
      center - (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size)),
      color);
    outerPoints[5] = Point(
      center + (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size)),
      color);
  }
  innerPoints.resize(6);
  {
    innerPoints[0] = Point(center + (pointDirection * small_size),
      color);
    innerPoints[1] = Point(
      center + (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size)),
      color);
    innerPoints[2] = Point(
      center - (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size)),
      color);
    innerPoints[3] = Point(center - (pointDirection * small_size),
      color);
    innerPoints[4] = Point(
      center - (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size)),
      color);
    innerPoints[5] = Point(
      center + (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size)),
      color);
  }
}
void HexagonGrid::Hexagon::connect_points(std::shared_ptr<Hexagon> ptr)
{
  innerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    innerPointsId[i] = connect(innerPoints[i], ptr);
  outerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    outerPointsId[i] = connect(outerPoints[i], ptr);
}
void HexagonGrid::Hexagon::set_color(Color color)
{
  Color last_color = center.color;
  center.color = color;
  for (int i = 0; i < 6; ++i)
  {
    Point p = Points::get_instance().get_point(innerPointsId[i]);
    p.color = color;
    Points::get_instance().update_point(innerPointsId[i], p);
    p = Points::get_instance().get_point(outerPointsId[i]);
    p.color = p.color - last_color + color;
    Points::get_instance().update_point(outerPointsId[i], p);
  }
}
void HexagonGrid::Hexagon::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  int ind = Vertices.size();
  Vertices.push_back(center);
  for (int i = 0; i < 6; ++i)
  {
    TriList.push_back(ind);
    TriList.push_back(innerPointsId[i]);
    TriList.push_back(innerPointsId[(i + 1)%6]);

    TriList.push_back(innerPointsId[(i + 1) % 6]);
    TriList.push_back(innerPointsId[i]);
    TriList.push_back(ind);
  }
}

//struct Triangle------------------------------------------

HexagonGrid::Triangle::Triangle(Point a, Point b, Point c){
  std::vector<uint32_t> ids = { 
    Points::get_instance().get_id_point(a) , 
    Points::get_instance().get_id_point(b) , 
    Points::get_instance().get_id_point(c) };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

HexagonGrid::Triangle::Triangle(uint32_t aId, uint32_t bId, uint32_t cId)
{
  std::vector<uint32_t> ids = { aId , bId , cId };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

bool HexagonGrid::Triangle::operator<(const Triangle& rhs) const
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

void HexagonGrid::Triangle::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList,
  const HexagonGrid& hexagonGrid) const {
  Point a = Points::get_instance().get_point(AId);
  Point b = Points::get_instance().get_point(BId);
  Point c = Points::get_instance().get_point(CId);
  
  int32_t heightA =
    round(hexagonGrid.heightDirection_.dot(
      a.position - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);
  int32_t heightB =
    round(hexagonGrid.heightDirection_.dot(
      b.position - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);
  int32_t heightC =
    round(hexagonGrid.heightDirection_.dot(
      c.position - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);

  std::vector<std::pair<int32_t, Point>> points;
  points.push_back({ heightA, a });
  points.push_back({ heightB, b });
  points.push_back({ heightC, c });
  std::sort(points.begin(), points.end());

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
        (hexagonGrid.nTerracesOnHeightStep_ + 1) - 1;
      Eigen::Vector3d step =
        (points[2].second.position -
          points[0].second.position) / (nTerraces + 1);
      Eigen::Vector3d platformVector =
        hexagonGrid.heightDirection_
        .cross(step)
        .cross(hexagonGrid.heightDirection_)
        .normalized();
      platformVector *= step.dot(platformVector) / 3;
      
      Eigen::Vector3d goal = points[0].second.position +
        step * (hexagonGrid.nTerracesOnHeightStep_ + 1)
        *(points[1].first - points[0].first);

      double len = (points[0].second.position
        - points[2].second.position).norm();

      Point p1;
      p1.position = goal - platformVector;
      p1.color = Color(points[0].second.color.get_abgr(),
        abs((p1.position - points[2].second.position).norm() 
        / len * 100) + 1) +
        Color(points[2].second.color.get_abgr(),
          abs((p1.position - points[0].second.position).norm() 
          / len * 100) + 1);

      Point p2;
      p2.position = goal + platformVector;
      p2.color = Color(points[0].second.color.get_abgr(),
        (p2.position - points[2].second.position).norm()
        / len * 100 + 1) +
        Color(points[2].second.color.get_abgr(),
          (p2.position - points[0].second.position).norm()
          / len * 100 + 1);


      print_stair(points[1].second, p1, points[0].second,
        points[1].second.position, goal, points[0].second.position,
        hexagonGrid, Vertices, TriList);
      print_stair(points[1].second, p2, points[2].second,
        points[1].second.position, goal, points[2].second.position,
        hexagonGrid, Vertices, TriList);

      printTri(points[1].second, p1, p2, 
        Vertices, TriList);
    }
    else
    {
      if (points[0].first == points[1].first)
        print_stair(points[0].second,
          points[1].second,
          points[2].second,
          points[0].second.position,
          points[1].second.position,
          points[2].second.position,
          hexagonGrid,
          Vertices, TriList);
      else
        print_stair(points[1].second,
          points[2].second,
          points[0].second,
          points[1].second.position,
          points[2].second.position,
          points[0].second.position,
          hexagonGrid,
          Vertices, TriList);
    }
  }
}

void HexagonGrid::Triangle::print_stair(Point a, Point b, Point c,
  Eigen::Vector3d a_goal, 
  Eigen::Vector3d b_goal, 
  Eigen::Vector3d c_goal,
  const HexagonGrid& hexagonGrid,
  std::vector<PrintingPoint>& Vertices, 
  std::vector<uint16_t>& TriList) const
{
  Point d = c;
  Eigen::Vector3d d_goal = c_goal;
  int32_t heightstart =
    round(hexagonGrid.heightDirection_.dot(
      a_goal - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);
  int32_t heightend =
    round(hexagonGrid.heightDirection_.dot(
      c_goal - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(b, d);
    std::swap(a_goal, c_goal);
    std::swap(b_goal, d_goal);
    std::swap(heightstart, heightend);
  }

  std::vector<std::pair<Point, Point>> stairs;
  stairs.push_back({ a, b });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      (heightend - heightstart) *
      (hexagonGrid.nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c_goal - a_goal) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      hexagonGrid.heightDirection_
      .cross(step1)
      .cross(hexagonGrid.heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;

    Eigen::Vector3d step2 =
      (d_goal - b_goal) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      hexagonGrid.heightDirection_
      .cross(step2)
      .cross(hexagonGrid.heightDirection_)
      .normalized();
    platformVector2 *= step2.dot(platformVector2) / 3;
    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a.position - c.position).norm();
      Point p1, p2, p3, p4;

      p1.position = a_goal + step1 * i - platformVector1;
      p2.position = b_goal + step2 * i - platformVector2;
      p3.position = a_goal + step1 * i + platformVector1;
      p4.position = b_goal + step2 * i + platformVector2;

      p1.color = Color(c.color.get_abgr(),
        abs((p1.position - a_goal).norm() / len * 100) + 1) +
        Color(a.color.get_abgr(),
          abs((p1.position - c_goal).norm() / len * 100) + 1);

      p2.color = Color(d.color.get_abgr(),
        abs((p2.position - b_goal).norm() / len * 100) + 1) +
        Color(b.color.get_abgr(),
          abs((p2.position - d_goal).norm() / len * 100) + 1);

      p3.color = Color(c.color.get_abgr(),
        abs((p3.position - a_goal).norm() / len * 100) + 1) +
        Color(a.color.get_abgr(),
          abs((p3.position - c_goal).norm() / len * 100) + 1);

      p4.color = Color(d.color.get_abgr(),
        abs((p4.position - b_goal).norm() / len * 100) + 1) +
        Color(b.color.get_abgr(),
          abs((p4.position - d_goal).norm() / len * 100) + 1);

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], Vertices, TriList);
}

//struct Rectangle------------------------------------------

HexagonGrid::BorderRectangle::BorderRectangle(Point a, Point b,
  Point c, Point d) {
  AId = Points::get_instance().get_id_point(a);
  BId = Points::get_instance().get_id_point(b);
  CId = Points::get_instance().get_id_point(c);
  DId = Points::get_instance().get_id_point(d);
}

HexagonGrid::BorderRectangle::BorderRectangle(
  uint32_t aId, uint32_t bId,
  uint32_t cId, uint32_t dId)
{
  AId = aId;
  BId = bId;
  CId = cId;
  DId = dId;
}

bool HexagonGrid::BorderRectangle::operator<(const BorderRectangle& rhs) const
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

void HexagonGrid::BorderRectangle::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices,
  std::vector<uint16_t>& TriList,
  const HexagonGrid& hexagonGrid) const {
  Point a = Points::get_instance().get_point(AId);
  Point b = Points::get_instance().get_point(BId);
  Point c = Points::get_instance().get_point(CId);
  Point d = Points::get_instance().get_point(DId);

  int32_t heightstart =
    round(hexagonGrid.heightDirection_.dot(
      a.position - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);
  int32_t heightend =
    round(hexagonGrid.heightDirection_.dot(
      c.position - hexagonGrid.origin_) /
      hexagonGrid.heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(b, d);
    std::swap(heightstart, heightend);
  }

  std::vector<std::pair<Point, Point>> stairs;
  stairs.push_back({ a, b });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      (heightend - heightstart) *
      (hexagonGrid.nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c.position - a.position) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      hexagonGrid.heightDirection_
      .cross(step1)
      .cross(hexagonGrid.heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;
    Eigen::Vector3d step2 =
      (d.position - b.position) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      hexagonGrid.heightDirection_
      .cross(step2)
      .cross(hexagonGrid.heightDirection_)
      .normalized();
    platformVector2 *= step2.dot(platformVector2) / 3;
    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a.position - c.position).norm();
      Point p1 = a + step1 * i - platformVector1;
      Point p2 = b + step2 * i - platformVector2;
      Point p3 = a + step1 * i + platformVector1;
      Point p4 = b + step2 * i + platformVector2;

      p1.color = Color(c.color.get_abgr(),
        (p1.position - a.position).norm()/len * 100) +
        Color(a.color.get_abgr(), 
        (p1.position - c.position).norm()/len * 100);
      p2.color = Color(d.color.get_abgr(),
        (p2.position - b.position).norm() / len * 100) +
        Color(b.color.get_abgr(),
          (p2.position - d.position).norm() / len * 100);
      p3.color = Color(c.color.get_abgr(),
        (p3.position - a.position).norm() / len * 100) +
        Color(a.color.get_abgr(),
          (p3.position - c.position).norm() / len * 100);
      p4.color = Color(d.color.get_abgr(),
        (p4.position - b.position).norm() / len * 100) +
        Color(b.color.get_abgr(),
          (p4.position - d.position).norm() / len * 100);

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], Vertices, TriList);
}

//struct HexagonGrid---------------------------------------
HexagonGrid::HexagonGrid(float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, Eigen::Vector3d  col_direction,
  uint32_t n_rows, uint32_t n_cols, Color color,
  float height_step, uint32_t n_terraces_on_height_step)
  : origin_(origin), size_(size), innerSize_(0.75 * size),
  heightStep_(height_step), 
  nTerracesOnHeightStep_(n_terraces_on_height_step)
{
  if (abs( col_direction.dot(row_direction)) > PRECISION_DBL_CALC)
    throw std::invalid_argument(" ol_direction and row_direction not perpendicular");

  row_direction.normalize();
   col_direction.normalize();
  
  rowDirection_ = row_direction;
  colDirection_ =  col_direction;
  heightDirection_ = rowDirection_.cross(colDirection_);

  for (int row = 0; row < n_rows; ++row)
  {
    for (int col = 0; col < n_cols; ++col)
    {
      add_hexagon(row, col, color);
    }
  }
}
void HexagonGrid::add_hexagon(uint32_t row, uint32_t col,
  Color color)
{
  if (grid_.find(Coord(row, col)) != grid_.end())
    return;

  std::shared_ptr<Hexagon> ptr =
    std::make_shared<Hexagon>(size_, innerSize_,
      origin_ + 
       colDirection_ * 
      ((sqrtf(3) * col + (row % 2) * sqrt(3) / 2) * size_)
      + rowDirection_ * 
      (1.5 * size_ * row),
      rowDirection_,  colDirection_, color);
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
      triangles.insert(Triangle(
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
        rectangles.insert(BorderRectangle(
          hex1_inds_common_vertex[0],
          hex1_inds_common_vertex[1],
          hex2_inds_common_vertex[0],
          hex2_inds_common_vertex[1]
        ));
      }
  }
}
void HexagonGrid::generate_random_field()
{
  srand(time(0));
  for (const auto& [coord, hex] : grid_)
  {
    hex->set_color(Color(rand() % 255, rand() % 255, rand() % 255));
    set_height(coord.row, coord.col, rand() % 3 - 1);
  }
}
void HexagonGrid::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  Points::get_instance().print_in_vertices(Vertices);
  for (const auto& [coord, hex] : grid_)
    hex->print_in_vertices_and_triList(Vertices, TriList);
  for (const auto& triangle : triangles)
    triangle.print_in_vertices_and_triList(Vertices, TriList, *this);
  for (const auto& rectangle : rectangles)
    rectangle.print_in_vertices_and_triList(Vertices, TriList, *this);
}
void HexagonGrid::set_height(int row, int col, int32_t height)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  Point& center = grid_.at(Coord(row, col))->center;
  int32_t last_height = 
    round(heightDirection_.dot(center.position - origin_) / 
    heightStep_);
  center += (height - last_height) * heightStep_ * heightDirection_;
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_.at(Coord(row, col))->innerPointsId[i];
    
    Point p = Points::get_instance().get_point(id);
    last_height = 
      round(heightDirection_.dot(p.position - origin_) /
       heightStep_);
    p += (height - last_height) * heightStep_ * heightDirection_;

    Points::get_instance().update_point(id, p);
  }
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_.at(Coord(row, col))->outerPointsId[i];
    Point p = Points::get_instance().get_point(id);

    Eigen::Matrix3d positions;
    int cnt = 0;
    for (const auto obj : Points::get_instance().get_objects(id))
      if (obj->is_hexagon())
      {
        const Hexagon& hex = *std::static_pointer_cast<Hexagon>(obj);
        positions(cnt, 0) = hex.center.position.x();
        positions(cnt, 1) = hex.center.position.y();
        positions(cnt, 2) = hex.center.position.z();
        cnt++;
      }

    p.position = Eigen::Vector3d(
      positions.col(0).mean(),
      positions.col(1).mean(),
      positions.col(2).mean());

    Points::get_instance().update_point(id, p);
  }
}
void HexagonGrid::set_color(int row, int col, Color color)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  grid_.at(Coord(row, col))->set_color(color);
}
