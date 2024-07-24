#include <hexoworld/hexoworld.hpp>
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
  for (auto& p : base->frames[Usual]->get_points())
    Points::get_instance().set_point_color(p, color_, base->base);
}