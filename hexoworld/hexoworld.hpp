#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <Eigen/Dense>
#define PRECISION_DBL_CALC 0.0001 

class Color {
public:
  Color();
  Color(uint8_t red, uint8_t blue,
    uint8_t green, uint8_t alpha = 255, uint32_t n_parts = 1);
  Color(uint32_t abgr, uint32_t n_parts = 1);
  Color operator+ (const Color& rhs) const;
  Color operator- (const Color& rhs) const;
  uint32_t get_abgr() const;
private:
  uint32_t abgr_;
  uint32_t n_parts_;
};

struct Point
{
  Point();
  Point(Eigen::Vector3d pos, Color color = Color(0, 0, 0));
  Point(double x, double y, double z,
    Color color = Color(0, 0, 0));
  Point operator+ (Eigen::Vector3d v);
  Point operator- (Eigen::Vector3d v);
  Point& operator+= (Eigen::Vector3d v);
  bool operator< (const Point& rhs) const;
  Eigen::Vector3d position;
  Color color;
};

struct PrintingPoint {
  PrintingPoint(Point p);
  float x, y, z;
  uint32_t abgr;
};

struct Object {
  virtual bool is_hexadon() { return false; }
  virtual bool is_triangle() { return false; }
  virtual bool is_rectangle() { return false; }
};
struct Coord {
  bool operator< (const Coord& rhs) const
  {
    return (row < rhs.row) ||
      ((row == rhs.row) && (col < rhs.col));
  }
  uint32_t row, col;
};
class Points {
public:
  static Points& get_instance() {
    static Points instance;
    return instance;
  }
  uint32_t connect_point_with_object(Point p,
    std::shared_ptr<Object> object);
  bool in_points(Point p) const;
  uint32_t get_id_point(Point p) const;
  std::vector<std::shared_ptr<Object>> get_objects(uint32_t id);
  Point get_point(uint32_t id) const;
  void update_point(uint32_t id, Point new_point);
  void print_in_vertices(std::vector<PrintingPoint>& Vertices) {
    for (const Point& p : id_to_point)
      Vertices.push_back(p);
  }
private:
  std::map<Point, uint32_t> point_to_id;
  std::vector<Point> id_to_point;
  std::vector<std::vector<std::shared_ptr<Object>>> id_to_objects;
};

struct Hexagon : Object {
  bool is_hexadon() { return true; }
  Hexagon() {}
  Hexagon(float big_size, float small_size, Eigen::Vector3d center,
    Eigen::Vector3d pointDerection, Eigen::Vector3d floatDerection,
    Color color = Color(0, 0, 0));
  void connect_points(std::shared_ptr<Hexagon> ptr);
  void set_color(Color color);
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  std::vector<uint32_t> innerPointsId, outerPointsId;
  Point center;
  std::vector<Point> innerPoints;
  std::vector<Point> outerPoints;
};

struct HeightData {
  Eigen::Vector3d heightDirection_;
  Eigen::Vector3d origin_;
  float heightStep_;
  int nTerracesOnHeightStep_;
};

class Triangle : Object {
public:
  bool is_triangle() { return true; }
  Triangle(Point a, Point b, Point c, HeightData heightData);
  Triangle(uint32_t aId, uint32_t bId, uint32_t cId,
    HeightData heightData);
  bool operator< (const Triangle& rhs) const;
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;
  uint32_t AId, BId, CId;
private:
  void print_stair(Point a, Point b, Point c,
    Eigen::Vector3d a_goal,
    Eigen::Vector3d b_goal,
    Eigen::Vector3d c_goal,
    HeightData heightData_,
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;
  HeightData heightData_;
};

class BorderRectangle : Object {
public:
  bool is_rectangle() { return true; }
  BorderRectangle(Point a, Point b, Point c, Point d,
    HeightData heightData);
  BorderRectangle(uint32_t aId, uint32_t bId,
    uint32_t cId, uint32_t dId, HeightData heightData);
  bool operator< (const BorderRectangle& rhs) const;
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;
  uint32_t AId, BId, CId, DId;
private:
  HeightData heightData_;
};

class HexagonGrid {
public:
  HexagonGrid(float size, Eigen::Vector3d origin,
    Eigen::Vector3d row_direction, Eigen::Vector3d ñol_direction,
    uint32_t n_rows = 0, uint32_t n_cols = 0,
    Color color = Color(0, 0, 0),
    float height_step = 1.0f, int n_terraces_on_height_step = 2);
  void add_hexagon(uint32_t row, uint32_t col,
    Color color = Color(0, 0, 0));

  void generate_random_field();

  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  void set_height(int row, int col, int32_t height);
  void set_color(int row, int col, Color color);

private:
  std::map<Coord, std::shared_ptr<Hexagon>> grid_;
  std::set<Triangle> triangles;
  std::set<BorderRectangle> rectangles;
  Eigen::Vector3d rowDirection_;
  Eigen::Vector3d ñolDirection_;
  Eigen::Vector3d heightDirection_;
  Eigen::Vector3d origin_;
  float size_, innerSize_;
  HeightData heightData_;
};