#pragma once
#include <stdint.h>
#include <vector>
#include <map>

uint32_t color(uint8_t red, uint8_t blue, 
  uint8_t green, uint8_t alpha = 255);

class Color {
public:
  Color();
  Color(uint8_t red, uint8_t blue,
    uint8_t green, uint8_t alpha = 255, uint32_t n_parts = 1);
  Color operator+ (const Color& rhs) const;
  Color operator- (const Color& rhs) const;
  uint32_t get_abgr() const;
private:
  uint32_t abgr_;
  uint32_t n_parts_;
};

struct Vector;
struct Point
{
  Point();
  Point(float x, float y, float z, Color abgr = Color(0, 0, 0));
  Point operator+ (Vector v);
  Point operator- (Vector v);
  Point& operator+= (Vector v);
  bool operator< (const Point& rhs) const;
  float x, y, z;
  Color color;
};

struct PrintingPoint {
  PrintingPoint(Point p);
  float x, y, z;
  uint32_t abgr;
};

struct Coord {
  uint32_t row, col;
};
class Points {
public:
  static Points& get_instance() {
    static Points instance;
    return instance;
  }
  uint32_t connect_point_with_hex(Point p, Coord coord);
  uint32_t get_id_point(Point p);
  std::vector<Coord> get_hexs(uint32_t id);
  Point get_point(uint32_t id);
  void update_point(uint32_t id, Point new_point);
  void print_in_vertices(std::vector<PrintingPoint>& Vertices) {
    for (const Point& p : id_to_point)
      Vertices.push_back(p);
  }
private:
  std::map<Point, uint32_t> point_to_id;
  std::vector<Point> id_to_point;
  std::vector<std::vector<Coord>> id_to_hexs;
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

struct Hexagon {
  Hexagon() {}
  Hexagon(float size, Point center, Vector pointTop, Vector floatTop, Coord coord);
  void SetColor(Color abgr);
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  std::vector<uint32_t> pointsId;
  Point center;
};

struct HexagonGrid {
  HexagonGrid(float size, Point origin, 
    Vector row_direction, Vector ñol_direction,
    uint32_t n_rows, uint32_t n_cols);
  void generate_random_field();
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;
  void set_height(int row, int col, float height);
  
  std::vector<std::vector<Hexagon>> grid_;
  Vector row_direction_;
  Vector ñol_direction_;
  Vector height_direction_;
  Point origin_;
  uint32_t n_rows_;
  uint32_t n_cols_;
};