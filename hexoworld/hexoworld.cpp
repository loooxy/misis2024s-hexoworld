#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

Color::Color() : Color(0, 0, 0) {}
Color::Color(uint8_t red, uint8_t blue,
  uint8_t green, uint8_t alpha, uint32_t n_parts)
  : n_parts_(n_parts) {
  abgr_ = (uint32_t(red) << 0) |
    (uint32_t(blue) << 8) |
    (uint32_t(green) << 16) |
    (uint32_t(alpha) << 24);
}
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

uint32_t connect(Point point, std::shared_ptr<Object> object)
{
  return Points::get_instance().connect_point_with_object(point, object);
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
  return Point(position - v, color);
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
uint32_t Points::connect_point_with_object(Point p,
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

uint32_t Points::get_id_point(Point p)
{
  if (point_to_id.find(p) == point_to_id.end())
    throw std::invalid_argument("unknown point");
  return point_to_id[p];
}
std::vector<std::shared_ptr<Object>> Points::get_objects(uint32_t id)
{
  if (id >= id_to_objects.size())
    throw std::invalid_argument("wrong id");
  return id_to_objects[id];
}
Point Points::get_point(uint32_t id)
{
  if (id >= id_to_point.size())
    throw std::invalid_argument("wrong id");
  return id_to_point[id];
}
void Points::update_point(uint32_t id, Point new_point)
{
  Point point = Points::get_instance().id_to_point[id];
  Points::get_instance().point_to_id.erase(point);
  Points::get_instance().point_to_id[new_point] = id;
  Points::get_instance().id_to_point[id] = new_point;
}

//struct Hexagon-------------------------------------------

Hexagon::Hexagon(float big_size, float small_size, Point center,
  Eigen::Vector3d pointDerection, Eigen::Vector3d floatDerection) :
  center(center) {
  if (abs(pointDerection.dot(floatDerection)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");

  pointDerection.normalize();
  floatDerection.normalize();
  outerPoints.resize(6);
  {
    outerPoints[0] = center + (pointDerection * big_size);
    outerPoints[1] =
      center + (pointDerection * (0.5 * big_size)) +
      (floatDerection * (sqrtf(3) / 2 * big_size));
    outerPoints[2] =
      center - (pointDerection * (0.5 * big_size)) +
      (floatDerection * (sqrtf(3) / 2 * big_size));
    outerPoints[3] = center - (pointDerection * big_size);
    outerPoints[4] =
      center - (pointDerection * (0.5 * big_size)) -
      (floatDerection * (sqrtf(3) / 2 * big_size));
    outerPoints[5] =
      center + (pointDerection * (0.5 * big_size)) -
      (floatDerection * (sqrtf(3) / 2 * big_size));
  }
  innerPoints.resize(6);
  {
    innerPoints[0] = center + (pointDerection * small_size);
    innerPoints[1] =
      center + (pointDerection * (0.5 * small_size)) +
      (floatDerection * (sqrtf(3) / 2 * small_size));
    innerPoints[2] =
      center - (pointDerection * (0.5 * small_size)) +
      (floatDerection * (sqrtf(3) / 2 * small_size));
    innerPoints[3] = center - (pointDerection * small_size);
    innerPoints[4] =
      center - (pointDerection * (0.5 * small_size)) -
      (floatDerection * (sqrtf(3) / 2 * small_size));
    innerPoints[5] =
      center + (pointDerection * (0.5 * small_size)) -
      (floatDerection * (sqrtf(3) / 2 * small_size));
  }
}
void Hexagon::connect_points(std::shared_ptr<Hexagon> ptr)
{
  innerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    innerPointsId[i] = connect(innerPoints[i], ptr);
  outerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    outerPointsId[i] = connect(outerPoints[i], ptr);
}
void Hexagon::set_color(Color abgr)
{
  center.color = abgr;
  for (int i = 0; i < 6; ++i)
  {
    Point p = Points::get_instance().get_point(innerPointsId[i]);
    p.color = abgr;
    Points::get_instance().update_point(innerPointsId[i], p);
  }
}
void Hexagon::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
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

Triangle::Triangle(Point a, Point b, Point c) {
  std::vector<uint32_t> ids = {
    Points::get_instance().get_id_point(a) ,
    Points::get_instance().get_id_point(b) ,
    Points::get_instance().get_id_point(c) };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

Triangle::Triangle(uint32_t aId, uint32_t bId, uint32_t cId)
{
  std::vector<uint32_t> ids = { aId , bId , cId };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

bool Triangle::operator<(const Triangle& rhs) const
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

void Triangle::print_in_triList(
  std::vector<uint16_t>& TriList) const {
  TriList.push_back(AId);
  TriList.push_back(BId);
  TriList.push_back(CId);

  TriList.push_back(CId);
  TriList.push_back(BId);
  TriList.push_back(AId);
}

//struct Rectangle------------------------------------------

BorderRectangle::BorderRectangle(Point a, Point b,
  Point c, Point d) {
  AId = Points::get_instance().get_id_point(a);
  BId = Points::get_instance().get_id_point(b);
  CId = Points::get_instance().get_id_point(c);
  DId = Points::get_instance().get_id_point(d);
}

BorderRectangle::BorderRectangle(uint32_t aId, uint32_t bId,
  uint32_t cId, uint32_t dId)
{
  AId = aId;
  BId = bId;
  CId = cId;
  DId = dId;
}

bool BorderRectangle::operator<(const BorderRectangle& rhs) const
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

void BorderRectangle::print_in_triList(
  std::vector<uint16_t>& TriList) const {

  Triangle(AId, BId, CId).print_in_triList(TriList);
  Triangle(CId, DId, BId).print_in_triList(TriList);
}

//struct HexagonGrid---------------------------------------
HexagonGrid::HexagonGrid(float size, Point origin,
  Eigen::Vector3d row_direction, Eigen::Vector3d ñol_direction,
  uint32_t n_rows, uint32_t n_cols)
  : origin_(origin), size_(size), inner_size_(0.75 * size)
{
  if (abs(ñol_direction.dot(row_direction)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("ñol_direction and row_direction not perpendicular");

  row_direction.normalize();
  ñol_direction.normalize();
  row_direction_ = row_direction;
  ñol_direction_ = ñol_direction;
  height_direction_ = row_direction_.cross(ñol_direction_);

  for (int row = 0; row < n_rows; ++row)
  {
    for (int col = 0; col < n_cols; ++col)
    {
      add_hexagon(row, col);
    }
  }
}
void HexagonGrid::add_hexagon(uint32_t row, uint32_t col)
{
  if (grid_.find(Coord(row, col)) != grid_.end())
    return;

  std::shared_ptr<Hexagon> ptr =
    std::make_shared<Hexagon>(size_, inner_size_,
      origin_ +
      ñol_direction_ *
      ((sqrtf(3) * col + (row % 2) * sqrt(3) / 2) * size_)
      + row_direction_ *
      (1.5 * size_ * row),
      row_direction_, ñol_direction_);
  grid_[Coord(row, col)] = ptr;
  ptr->connect_points(ptr);

  for (int vertex = 0; vertex < 6; ++vertex)
  {
    uint32_t id = ptr->outerPointsId[vertex];
    std::vector<std::shared_ptr<Object>> objs =
      Points::get_instance().get_objects(id);
    std::vector<std::shared_ptr<Hexagon>> hexs;
    for (const auto& obj : objs)
      if (obj->is_hexadon())
        hexs.push_back(std::static_pointer_cast<Hexagon>(obj));

    //init triangles-------------------------------------
    std::vector<uint32_t> idInnerPoints;
    for (const auto& hex : hexs)
      for (int j = 0; j < 6; ++j)
        if (hex->outerPointsId[j] == id)
          idInnerPoints.push_back(hex->innerPointsId[j]);

    if (idInnerPoints.size() == 3)
      triangles.insert(Triangle(
        idInnerPoints[0], idInnerPoints[1], idInnerPoints[2]
      ));

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
void HexagonGrid::set_random_colors()
{
  srand(time(0));
  for (const auto& [coord, hex] : grid_)
    hex->set_color(Color(rand() % 255, rand() % 255, rand() % 255));
}
void HexagonGrid::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  Points::get_instance().print_in_vertices(Vertices);
  for (const auto& [coord, hex] : grid_)
    hex->print_in_vertices_and_triList(Vertices, TriList);
  for (const auto& triangle : triangles)
    triangle.print_in_triList(TriList);
  for (const auto& rectangle : rectangles)
    rectangle.print_in_triList(TriList);
}
void HexagonGrid::set_height(int row, int col, float height)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  grid_.at(Coord(row, col))->center += (height_direction_ * height);
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_.at(Coord(row, col))->innerPointsId[i];

    Point new_p = Points::get_instance().get_point(id) +
      (height_direction_ * height);

    Points::get_instance().update_point(id, new_p);
  }
}