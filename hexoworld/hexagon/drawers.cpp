#include <hexoworld/hexoworld.hpp>

Hexoworld::Hexagon::HexagonDrawer::HexagonDrawer(Object* object, Eigen::Vector4i color)
  : Drawer(object, color) {}

Hexoworld::Hexagon::RiverDrawer::RiverDrawer(Object* object, Eigen::Vector4i color)
  : Drawer(object, color) {
}
void Hexoworld::Hexagon::RiverDrawer::colorize_points()
{
  for (auto& point : base->frame->get_points())
    Points::get_instance().set_point_color(point, color_, base);

  std::shared_ptr<RiversFrame> type = std::static_pointer_cast<RiversFrame>(base->frame);

  for (const auto& point : type->get_water_points())
    Points::get_instance().set_point_color(point, base->world.riverColor, base);
}
