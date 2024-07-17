#include <hexoworld/hexoworld.hpp>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const double Hexoworld::PRECISION_DBL_CALC = 0.0000001;

Hexoworld::Hexoworld(
  float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, 
  Eigen::Vector3d сol_direction, 
  float height_step,
  uint32_t n_terraces_on_height_step,
  uint32_t n_rows, uint32_t n_cols)
  : origin_(origin), 
  rowDirection_(row_direction.normalized()), 
  colDirection_(сol_direction.normalized()),
  heightDirection_(row_direction.cross(сol_direction).normalized()),
  size_(size), innerSize_(0.75 * size), 
  heightStep_(height_step), 
  nTerracesOnHeightStep_(n_terraces_on_height_step)
{
  if (abs(row_direction.dot(сol_direction)) > PRECISION_DBL_CALC)
    throw std::invalid_argument(
      "row_direction and сol_direction not perpendicular");

  manager = std::make_unique<Manager>(*this);

  for (int row = 0; row < n_rows; ++row)
  {
    for (int col = 0; col < n_cols; ++col)
    {
      add_hexagon(row, col,
        Eigen::Vector4i(255, 255, 255, 0));
    }
  }
}

void Hexoworld::add_hexagon(uint32_t row, uint32_t col,
  Eigen::Vector4i color)
{
  Coord pos(row, col);
  std::shared_ptr<Hexagon> hex = manager->get_hexagon(pos);
  hex->drawer->set_color(color);

  std::set<Coord> neighbors = manager->get_neighbors(pos);
  for (Coord pos_neighbor : neighbors)
  {
    manager->get_rectangle(pos, pos_neighbor)->drawer->set_color(color);

    std::set<Coord> neighbors_second = manager->get_neighbors(pos_neighbor);
    std::vector<Coord> intersection;
    std::set_intersection(neighbors.begin(), neighbors.end(),
      neighbors_second.begin(), neighbors_second.end(),
      std::back_inserter(intersection));
    for (Coord pos_third : intersection)
      manager->get_triangle(pos, pos_neighbor, pos_third)->drawer->set_color(color);
  }
}

void Hexoworld::add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs) {
  std::vector<Coord> positions;
  for (const auto& hex : hexs)
    positions.push_back(Coord(hex.first, hex.second));

  if (positions.size() <= 1)
    throw std::invalid_argument("So short river.");

  for (int i = 0; i < positions.size() - 1; ++i)
  {
    std::set<Coord> neighbors = manager->get_neighbors(positions[i]);
    if (neighbors.find(positions[i + 1]) == neighbors.end())
      throw std::invalid_argument("Cell " + std::to_string(i) + " and cell " + std::to_string(i + 1) + 
      " are not neighbors.");
  }

  for (int i = 0; i < positions.size(); ++i)
  {
    uint32_t before_ind, next_ind;
    if (i > 0)
    {
      before_ind = get_ind_direction(positions[i], positions[i - 1]);
      manager->get_rectangle(positions[i], positions[i - 1])->make_river();
    }
    else
      before_ind = -1;
    
    if (i < positions.size() - 1)
      next_ind = get_ind_direction(positions[i], positions[i + 1]);
    else
      next_ind = -1;

    manager->get_hexagon(positions[i])->make_river(before_ind, next_ind);
  }
}

void Hexoworld::set_hex_height(uint32_t row, uint32_t col, int32_t height)
{
  std::static_pointer_cast<Hexagon::HexagonFrame>(
    manager->get_hexagon(Coord(row, col))->frame)
    ->set_height(height);
}

void Hexoworld::set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color)
{
  Coord pos(row, col);
  manager->get_hexagon(pos)->drawer->set_color(color);

  Eigen::Vector4i a = color;
  std::set<Coord> neighbors = manager->get_neighbors(pos);
  for (Coord pos_neighbor : neighbors)
  {
    Eigen::Vector4i b = manager->get_hexagon(pos_neighbor)->drawer->get_color();
    manager->get_rectangle(pos, pos_neighbor)->drawer->set_color(a + (b - a) / 2);

    std::set<Coord> neighbors_second = manager->get_neighbors(pos_neighbor);
    std::vector<Coord> intersection;
    std::set_intersection(neighbors.begin(), neighbors.end(),
      neighbors_second.begin(), neighbors_second.end(),
      std::back_inserter(intersection));
    for (Coord pos_third : intersection)
    {
      Eigen::Vector4i c = manager->get_hexagon(pos_third)->drawer->get_color();
      
      double len_a = (b - a).norm() + (c - a).norm();
      double len_b = (b - a).norm() + (c - b).norm();
      double len_c = (b - c).norm() + (c - a).norm();
      double max_len = std::max(len_a, std::max(len_b, len_c));

      Eigen::Vector4i dop_color = (max_len == len_a ? a : (max_len == len_b ? b : c));

      Eigen::Matrix4i colors = (Eigen::MatrixXi(4, 4) << 
        a.x(), a.y(), a.z(), a.w(),
        b.x(), b.y(), b.z(), b.w(),
        c.x(), c.y(), c.z(), c.w(),
        dop_color.x(), dop_color.y(), dop_color.z(), dop_color.w()/10).finished();

      manager->get_triangle(pos, pos_neighbor, pos_third)->drawer->set_color(
      Eigen::Vector4i(
      colors.col(0).mean(),
      colors.col(1).mean(),
      colors.col(2).mean(),
      colors.col(3).mean()
      ));
    }
  }
}

void Hexoworld::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  for (auto& object : manager->get_all_object())
    object->print_in_triList(TriList);
  Points::get_instance().print_in_vertices(Vertices);
}