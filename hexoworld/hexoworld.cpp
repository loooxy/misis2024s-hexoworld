#include <hexoworld/hexoworld.hpp>
#include <hexoworld/texture_grid.hpp>
#include <hexoworld/hexagon_grid.hpp>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const double Hexoworld::PRECISION_DBL_CALC = 0.000001;

Hexoworld::Hexoworld(
  float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, Eigen::Vector3d сol_direction,
  uint32_t n_rows, uint32_t n_cols, float height_step,
  uint32_t n_terraces_on_height_step)
{
  hexagonGrid_ = std::make_unique<HexagonGrid>(
    *this, size, origin,
    row_direction, сol_direction,
    height_step, n_terraces_on_height_step);
  textureGrid_ = std::make_unique<TextureGrid>();
  
  srand(time(0));
  for (int row = 0; row < n_rows; ++row)
  {
    for (int col = 0; col < n_cols; ++col)
    {
      add_hexagon(row, col,
        Eigen::Vector4i(
          rand() % 256, rand() % 256, rand() % 256, 255));
    }
  }
}

void Hexoworld::add_hexagon(uint32_t row, uint32_t col,
  Eigen::Vector4i color)
{
  hexagonGrid_->add_hexagon(row, col);
  std::vector<Eigen::Vector3d> points = hexagonGrid_->get_hex_points(row, col);
  for (const auto& point : points)
    textureGrid_->set_point(point, color);
}

void Hexoworld::set_hex_height(uint32_t row, uint32_t col, int32_t height)
{
  hexagonGrid_->set_height(row, col, height);
}

void Hexoworld::set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color)
{
  std::vector<Eigen::Vector3d> points = hexagonGrid_->get_hex_points(row, col);
  for (const Eigen::Vector3d& point : points)
    textureGrid_->set_point(point, color);
}

void Hexoworld::print_in_vertices_and_triList(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  hexagonGrid_->print_in_vertices_and_triList(Vertices, TriList);
  textureGrid_->colorize_vertices(Vertices);
}