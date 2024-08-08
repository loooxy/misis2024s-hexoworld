#include <hexoworld/cottage/cottage.hpp>

Hexoworld::Cottage::WallsDrawer::WallsDrawer(FixedInventory* base)
  : CottageDrawer(base)
{
  color_ = Eigen::Vector4i(100, 149, 237, 255);
}

void Hexoworld::Cottage::WallsDrawer::colorize_points()
{
  for (auto& point : std::static_pointer_cast<WallsFrame>(base->frames[Walls])->get_pointsId())
    Points::get_instance().set_point_color(point, color_);
}

Hexoworld::Cottage::RoofDrawer::RoofDrawer(FixedInventory* base)
  : CottageDrawer(base)
{
  color_ = Eigen::Vector4i(255, 237, 192, 255);
}

void Hexoworld::Cottage::RoofDrawer::colorize_points()
{
  for (auto& point : std::static_pointer_cast<RoofFrame>(base->frames[Roof])->get_pointsId())
    Points::get_instance().set_point_color(point, color_);
}
