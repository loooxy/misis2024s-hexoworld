#include <hexoworld/hexoworld.hpp>
#include <map>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

const double Hexoworld::PRECISION_DBL_CALC = 0.0001;

Hexoworld::TextureGrid::Color::Color() : Color(0, 0, 0) {}
Hexoworld::TextureGrid::Color::Color(uint8_t red, uint8_t blue,
  uint8_t green, uint8_t alpha, uint32_t n_parts)
  : n_parts_(n_parts) {
  abgr_ = (uint32_t(red) << 0) |
    (uint32_t(blue) << 8) |
    (uint32_t(green) << 16) |
    (uint32_t(alpha) << 24);
}
Hexoworld::TextureGrid::Color::Color(uint32_t abgr, uint32_t n_parts)
  : abgr_(abgr), n_parts_(n_parts) {}

Hexoworld::TextureGrid::Color Hexoworld::TextureGrid::Color::operator+(const Color& rhs) const
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
uint32_t Hexoworld::TextureGrid::Color::get_abgr() const
{
  return abgr_;
}

void Hexoworld::TextureGrid::set_point(Eigen::Vector3d pos, Eigen::Vector4i color, uint32_t n_parts)
{
  points[pos] = Color(color.x(), color.y(), color.z(), color.w());
}

void Hexoworld::TextureGrid::colorize_vertices(std::vector<PrintingPoint>& Vertices) const 
{
  for (auto& v : Vertices)
    v.abgr = get_point_color(Eigen::Vector3d(v.x, v.y, v.z))
    .get_abgr();
}

Hexoworld::TextureGrid::Color Hexoworld::TextureGrid::get_point_color(Eigen::Vector3d position) const
{
  double min_dist = std::numeric_limits<double>::max();
  for (const auto& [pos, color] : points)
      min_dist = std::min(min_dist, (pos - position).norm());
  
  if (min_dist < PRECISION_DBL_CALC)
    return points.at(position);

  Color ans;
  for (const auto& [pos, color] : points)
  {
    double dist = (pos - position).norm();
    if (dist < min_dist * 3)
    {
      Color tmp(color.get_abgr(), int(min_dist * 100.0 / dist));
      ans = ans + tmp;
    }
  }
  return ans;
}
