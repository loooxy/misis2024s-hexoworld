#include "hexoworld.hpp"
#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>

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

uint32_t connect(Point point, Coord coord)
{
  return Points::get_instance().connect_point_with_hex(point, coord);
}

//struct Point---------------------------------------------
Point::Point() : x(0), y(0), z(0), color(0, 0, 0) {};
Point::Point(float x, float y, float z, 
  Color abgr)
  :x(x), y(y), z(z), color(abgr) {}
Point Point::operator+(Vector v)
{
  return Point(x + v.x, y + v.y, z + v.z, color);
}
Point Point::operator-(Vector v)
{
  return Point(x - v.x, y - v.y, z - v.z, color);
}
Point& Point::operator+=(Vector v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
bool Point::operator<(const Point& rhs) const
{
  float min_dif = 0.0001f;
  if (x + min_dif < rhs.x)
    return true;
  else if (x - min_dif > rhs.x)
    return false;

  if (y + min_dif < rhs.y)
    return true;
  else if (y - min_dif > rhs.y)
    return false;

  if (z + min_dif < rhs.z)
    return true;
  else
    return false;
}
//struct PrintingPoint-------------------------------------
PrintingPoint::PrintingPoint(Point p)
  : x(p.x), y(p.y), z(p.z), abgr(p.color.get_abgr()) {}
//class Points---------------------------------------------
uint32_t Points::connect_point_with_hex(Point p, Coord coord)
{
  uint32_t id;
  if (point_to_id.find(p) == point_to_id.end())
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_hexs.push_back(std::vector<Coord>());
    id_to_hexs.back().push_back(coord);
  }
  else
  {
    id = point_to_id[p];
    id_to_point[id].color = id_to_point[id].color + p.color;
    id_to_hexs[id].push_back(coord);
  }
  return id;
}

uint32_t Points::get_id_point(Point p)
{
  if (point_to_id.find(p) == point_to_id.end())
    throw std::invalid_argument("unknown point");
  return point_to_id[p];
}
std::vector<Coord> Points::get_hexs(uint32_t id)
{
  if (id >= id_to_hexs.size())
    throw std::invalid_argument("wrong id");
  return id_to_hexs[id];
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
//struct Vector--------------------------------------------
Vector::Vector() : x(0), y(0), z(0) {};
Vector::Vector(float x, float y, float z)
  :x(x), y(y), z(z){}
Vector::Vector(Point start, Point end)
: x(end.x - start.x), y(end.y - start.y), z(end.z - start.z) {}
float Vector::scalar_mult(Vector rhs)
{
  return x*rhs.x + y*rhs.y + z*rhs.z;
}
Vector Vector::vector_mult(Vector rhs)
{
  return Vector(
    y*rhs.z - z*rhs.y,
    z*rhs.x - x*rhs.z,
    x*rhs.y - y*rhs.x
    );
}
float Vector::lenght()
{
  return sqrtf(x*x + y*y + z*z);
}
Vector Vector::normalization()
{
  float len = lenght();
  return Vector(x/len, y/len, z/len);
}
Vector Vector::operator*(float k)
{
  return Vector(x*k, y*k, z*k);
}

//struct Hexagon-------------------------------------------

Hexagon::Hexagon(float size, Point center, 
  Vector pointTop, Vector floatTop, Coord coord) : center(center){
  if (abs(pointTop.scalar_mult(floatTop)) > 2 * FLT_MIN)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");
  
  Vector v = pointTop.normalization();
  Vector u = floatTop.normalization();
  pointsId.resize(6);
  pointsId[0] = connect(center + (v * size), coord);
  pointsId[1] = connect(
    center + (v * (0.5 * size)) + (u * (sqrtf(3) / 2 * size)),
    coord);
  pointsId[2] = connect(
    center - (v * (0.5 * size)) + (u * (sqrtf(3) / 2 * size)),
    coord);
  pointsId[3] = connect(center - (v * size), coord);
  pointsId[4] = connect(
    center - (v * (0.5 * size)) - (u * (sqrtf(3) / 2 * size)),
    coord);
  pointsId[5] = connect(
    center + (v * (0.5 * size)) - (u * (sqrtf(3) / 2 * size)), 
    coord);
}
void Hexagon::SetColor(Color abgr)
{
  Color last_color = center.color;
  center.color = abgr;
  for (int i = 0; i < 6; ++i)
  {
    Point p = Points::get_instance().get_point(pointsId[i]);
    p.color = p.color - last_color;
    p.color = p.color + center.color;
    Points::get_instance().update_point(pointsId[i], p);
  }
}
void Hexagon::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  int ind = Vertices.size();
  Vertices.push_back(center);
  for (int i = 0; i < 6; ++i)
  {
    TriList.push_back(ind);
    TriList.push_back(pointsId[i]);
    TriList.push_back(pointsId[(i + 1)%6]);

    TriList.push_back(pointsId[(i + 1) % 6]);
    TriList.push_back(pointsId[i]);
    TriList.push_back(ind);
  }
}

//struct HexagonGrid---------------------------------------
HexagonGrid::HexagonGrid(float size, Point origin,
  Vector row_direction, Vector ñol_direction,
  uint32_t n_rows, uint32_t n_cols) 
  : n_rows_(n_rows), n_cols_(n_cols), origin_(origin)
{
  if (abs(ñol_direction.scalar_mult(row_direction)) > 2 * FLT_MIN)
    throw std::invalid_argument("ñol_direction and row_direction not perpendicular");

  row_direction = row_direction.normalization();
  ñol_direction = ñol_direction.normalization();
  row_direction_ = row_direction;
  ñol_direction_ = ñol_direction;
  height_direction_ = row_direction_.vector_mult(ñol_direction_);

  grid_.resize(n_rows, std::vector<Hexagon>(n_cols));
  for (int row = 0; row < n_rows; ++row)
    for (int col = 0; col < n_cols; ++col)
    {
      grid_[row][col] = Hexagon(size,
        origin + ñol_direction * ((sqrtf(3) * col + (row % 2) * sqrt(3)/2) * size)
               + row_direction * (1.5 * size * row),
        row_direction, ñol_direction, Coord(row, col));
    }
}
void HexagonGrid::generate_random_field()
{
  srand(time(0));
  for (auto& i : grid_)
    for (auto& j : i)
      j.SetColor(Color(rand() % 255, rand() % 255, rand() % 255));
}
void HexagonGrid::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  Points::get_instance().print_in_vertices(Vertices);
  for (const auto& i : grid_)
    for (const auto& j : i)
      j.print_in_vertices_and_triList(Vertices, TriList);
}
void HexagonGrid::set_height(int row, int col, float height)
{
  if (!(0 <= row && row < n_rows_))
    throw std::invalid_argument("wrong row");
  if (!(0 <= col && col < n_cols_))
    throw std::invalid_argument("wrong col");

  grid_[row][col].center += (height_direction_ * height);  
  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = grid_[row][col].pointsId[i];
    std::vector<Coord> hexs = 
      Points::get_instance().get_hexs(id);
    
    Point sum_point;
    for (Coord hex : hexs)
    {
      sum_point.x += grid_[hex.row][hex.col].center.x;
      sum_point.y += grid_[hex.row][hex.col].center.y;
      sum_point.z += grid_[hex.row][hex.col].center.z;
    }
    sum_point.x /= hexs.size();
    sum_point.y /= hexs.size();
    sum_point.z /= hexs.size();
    
    Points::get_instance().update_point(id, sum_point);
  }
}
