#pragma once
#include <stdint.h>
#include <vector>
#include <map>

uint32_t color(uint8_t red, uint8_t blue, 
  uint8_t green, uint8_t alpha = 255);

struct Vector;
struct Point
{
  Point();
  Point(float x, float y, float z, uint32_t abgr);
  Point operator+ (Vector v);
  Point operator- (Vector v);
  Point& operator+= (Vector v);
  bool operator< (Point rhs);
  float x, y, z;
  uint32_t abgr;
};
struct Vector {
  Vector();
  Vector(float x, float y, float z);
  Vector(Point start, Point end);  
  float scalar_mult(Vector rhs);
  Vector vector_mult(Vector rhs);
  float lenght();
  Vector normalization();
  Vector operator* (float k);
  float x, y, z;
};

struct Triangle
{
  Triangle(Point A, Point B, Point C);
  void print_in_vertices_and_triList(
    std::vector<Point>& Vertices, 
    std::vector<uint16_t>& TriList) const;
  Point A, B, C;
};

struct Hexagon {
  Hexagon() {}
  Hexagon(float size, Point center, Vector pointTop, Vector floatTop);
  void SetColor(uint32_t abgr);
  void print_in_vertices_and_triList(
    std::vector<Point>& Vertices,
    std::vector<uint16_t>& TriList) const;
  std::vector<Point> points;
  Point center;
};

struct HexagonGrid {
  HexagonGrid(float size, Point origin, 
    Vector row_direction, Vector ñol_direction,
    uint32_t n_rows, uint32_t n_cols);
  void generate_random_field();
  void print_in_vertices_and_triList(
    std::vector<Point>& Vertices,
    std::vector<uint16_t>& TriList) const;
  void set_height(int row, int col, float height);
  
  std::vector<std::vector<Hexagon>> grid_;
  Vector row_direction_;
  Vector ñol_direction_;
  Vector height_direction_;
  Point origin_;
  uint32_t n_rows_;
  uint32_t n_cols_;
  std::map<Point, std::vector<std::pair<int, int>>> subordinate;
};