#include <hexoworld/wall/wall.hpp>
#include <random>
#include <cmath>

Hexoworld::Wall::WallDrawer::WallDrawer(FixedInventory* object)
  : Drawer(object)
{}

Hexoworld::Wall::UsualDrawer::UsualDrawer(FixedInventory* base, Eigen::Vector4i color)
  : WallDrawer(base), color_(color)
{
}

void Hexoworld::Wall::UsualDrawer::colorize_points()
{
  auto tmp = std::static_pointer_cast<UsualFrame>(base->frames[Usual])->get_pointsId();
  for (auto& p : tmp)
    Points::get_instance().set_point_color(p, color_);
}