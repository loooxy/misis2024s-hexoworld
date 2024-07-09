#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>

uint32_t color(uint8_t red, uint8_t blue,
  uint8_t green,uint8_t alpha) {
  return (uint32_t(red) << 0) |
    (uint32_t(blue) << 8) |
    (uint32_t(green) << 16) |
    (uint32_t(alpha) << 24);
};

//struct Point---------------------------------------------
Point::Point() : x(0), y(0), z(0) {};
Point::Point(float x, float y, float z, uint32_t abgr = color(0, 0, 0, 0))
  :x(x), y(y), z(z), abgr(abgr) {}
Point Point::operator+(Vector v)
{
  return Point(x + v.x, y + v.y, z + v.z, abgr);
}
Point Point::operator-(Vector v)
{
  return Point(x - v.x, y - v.y, z - v.z, abgr);
}
Point& Point::operator+=(Vector v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
bool Point::operator<(Point rhs)
{
  float min_dif = 2 * FLT_MIN;
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

//struct Triangle------------------------------------------
Triangle::Triangle(Point A, Point B, Point C)
: A(A), B(B), C(C){}
void Triangle::print_in_vertices_and_triList(
  std::vector<Point>& Vertices, std::vector<uint16_t>& TriList) const{
  int ind = Vertices.size();
  Vertices.push_back(A);
  Vertices.push_back(B);
  Vertices.push_back(C);

  TriList.push_back(ind);
  TriList.push_back(ind + 1);
  TriList.push_back(ind + 2);

  TriList.push_back(ind + 2);
  TriList.push_back(ind + 1);
  TriList.push_back(ind);
}

//struct Hexagon-------------------------------------------
Hexagon::Hexagon(float size, Point center, 
  Vector pointTop, Vector floatTop) : center(center){
  if (abs(pointTop.scalar_mult(floatTop)) > 2 * FLT_MIN)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");
  
  Vector v = pointTop.normalization();
  Vector u = floatTop.normalization();
  points.resize(6);
  points[0] = center + (v * size);
  points[1] = center + (v * (0.5 * size)) + (u * (sqrtf(3) / 2 * size));
  points[2] = center - (v * (0.5 * size)) + (u * (sqrtf(3) / 2 * size));
  points[3] = center - (v * size);
  points[4] = center - (v * (0.5 * size)) - (u * (sqrtf(3) / 2 * size));
  points[5] = center + (v * (0.5 * size)) - (u * (sqrtf(3) / 2 * size));
}
void Hexagon::SetColor(uint32_t abgr)
{
  for (int i = 0; i < 6; ++i)
    points[i].abgr = abgr;
  center.abgr = abgr;
}
void Hexagon::print_in_vertices_and_triList(std::vector<Point>& Vertices, std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < 5; ++i)
  {
    Triangle tmp(center, points[i], points[i + 1]);
    tmp.print_in_vertices_and_triList(Vertices, TriList);
  }
  Triangle tmp(center, points[5], points[0]);
  tmp.print_in_vertices_and_triList(Vertices, TriList);
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
        row_direction, ñol_direction);
    }
}
void HexagonGrid::generate_random_field()
{
  srand(time(0));
  for (auto& i : grid_)
    for (auto& j : i)
      j.SetColor(color(rand() % 255, rand() % 255, rand() % 255));
}
void HexagonGrid::print_in_vertices_and_triList(
  std::vector<Point>& Vertices, std::vector<uint16_t>& TriList) const
{
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
  if (row % 2 == 0)
  {
    //point0
    {
      Point sum_point = grid_[row][col].center;
      uint8_t cnt = 1;
      if (row + 1 < n_rows_ && col - 1 >= 0)
      {
        cnt++;
        sum_point.x += grid_[row + 1][col - 1].center.x;
        sum_point.y += grid_[row + 1][col - 1].center.y;
        sum_point.z += grid_[row + 1][col - 1].center.z;
      }
      if (row + 1 < n_rows_)
      {
        cnt++;
        sum_point.x += grid_[row + 1][col].center.x;
        sum_point.y += grid_[row + 1][col].center.y;
        sum_point.z += grid_[row + 1][col].center.z;
      }
      sum_point.x /= cnt;
      sum_point.y /= cnt;
      sum_point.z /= cnt;
      grid_[row][col].points[0] = sum_point;
      if (row + 1 < n_rows_ && col - 1 >= 0)
        grid_[row + 1][col - 1].points[2] = sum_point;
      if (row + 1 < n_rows_)
        grid_[row + 1][col].points[4] = sum_point;
    }
    //point1
    {
      Point sum_point = grid_[row][col].center;
      uint8_t cnt = 1;
      if (col + 1 < n_cols_)
      {
        cnt++;
        sum_point.x += grid_[row][col + 1].center.x;
        sum_point.y += grid_[row][col + 1].center.y;
        sum_point.z += grid_[row][col + 1].center.z;
      }
      if (row + 1 < n_rows_)
      {
        cnt++;
        sum_point.x += grid_[row + 1][col].center.x;
        sum_point.y += grid_[row + 1][col].center.y;
        sum_point.z += grid_[row + 1][col].center.z;
      }
      sum_point.x /= cnt;
      sum_point.y /= cnt;
      sum_point.z /= cnt;
      grid_[row][col].points[1] = sum_point;
      if (col + 1 < n_cols_)
        grid_[row][col + 1].points[5] = sum_point;
      if (row + 1 < n_rows_)
        grid_[row + 1][col].points[3] = sum_point;
    }
    //point2
    {
      Point sum_point = grid_[row][col].center;
      uint8_t cnt = 1;
      if (col + 1 < n_cols_)
      {
        cnt++;
        sum_point.x += grid_[row][col + 1].center.x;
        sum_point.y += grid_[row][col + 1].center.y;
        sum_point.z += grid_[row][col + 1].center.z;
      }
      if (row - 1 >= 0)
      {
        cnt++;
        sum_point.x += grid_[row - 1][col].center.x;
        sum_point.y += grid_[row - 1][col].center.y;
        sum_point.z += grid_[row - 1][col].center.z;
      }
      sum_point.x /= cnt;
      sum_point.y /= cnt;
      sum_point.z /= cnt;
      grid_[row][col].points[2] = sum_point;
      if (col + 1 < n_cols_)
        grid_[row][col + 1].points[4] = sum_point;
      if (row - 1 >= 0)
        grid_[row - 1][col].points[0] = sum_point;
    }
    //point3
    {
      Point sum_point = grid_[row][col].center;
      uint8_t cnt = 1;
      if (row - 1 >= 0)
      {
        cnt++;
        sum_point.x += grid_[row - 1][col].center.x;
        sum_point.y += grid_[row - 1][col].center.y;
        sum_point.z += grid_[row - 1][col].center.z;
      }
      if (row - 1 >= 0 && col - 1 >= 0)
      {
        cnt++;
        sum_point.x += grid_[row - 1][col - 1].center.x;
        sum_point.y += grid_[row - 1][col - 1].center.y;
        sum_point.z += grid_[row - 1][col - 1].center.z;
      }
      sum_point.x /= cnt;
      sum_point.y /= cnt;
      sum_point.z /= cnt;
      grid_[row][col].points[3] = sum_point;
      if (row - 1 >= 0)
        grid_[row - 1][col].points[5] = sum_point;
      if (row - 1 >= 0 && col - 1 >= 0)
        grid_[row - 1][col - 1].points[1] = sum_point;
    }
  }
  else
  {

  }
}
