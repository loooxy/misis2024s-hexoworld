#include <hexoworld/hexoworld.hpp>

void Hexoworld::Drawer::colorize_points()
{
  for (auto& point : base->frame->get_points())
    Points::get_instance().set_point_color(point, color_, base);
}
