#include <hexoworld/hexoworld.hpp>
#include <random>
#include <cmath>

Hexoworld::Hexagon::HexagonDrawer::HexagonDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Hexagon::UsualDrawer::UsualDrawer(Object* base, Eigen::Vector4i color)
  : HexagonDrawer(base), color_(color)
{}

void Hexoworld::Hexagon::UsualDrawer::colorize_points()
{
  std::shared_ptr<UsualFrame> frame = std::static_pointer_cast<UsualFrame>(base->frames[Usual]);

  for (const auto& point : frame->get_points())
    Points::get_instance().set_point_color(point, color_, base);
}

Hexoworld::Hexagon::RiverDrawer::RiverDrawer(Object* object)
  : HexagonDrawer(object) 
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-20, 20);
  auto make_new_component = [&gen, &dis](int32_t component) {
    component += dis(gen);
    return std::min(255, std::max(0, component));
    };

  special_color_river = Eigen::Vector4i(
    make_new_component(base->world.riverColor.x()),
    make_new_component(base->world.riverColor.y()),
    make_new_component(base->world.riverColor.z()),
    255
  );
}
void Hexoworld::Hexagon::RiverDrawer::colorize_points()
{
  std::shared_ptr<RiversFrame> frame = std::static_pointer_cast<RiversFrame>(base->frames[River]);

  for (const auto& point : frame->get_water_points())
    Points::get_instance().set_point_color(point, special_color_river, base);

  for (const auto& point : frame->get_shore_points())
    Points::get_instance().set_point_color(point, special_color_river, base);
}

void Hexoworld::Hexagon::RiverDrawer::set_new_special_color_river(Eigen::Vector4i new_color)
{
  special_color_river = new_color;
  for (const auto& point : std::static_pointer_cast<RiversFrame>(base->frames[River])->get_water_points())
    Points::get_instance().set_point_color(point, special_color_river, base);
}

Hexoworld::Hexagon::FloodDrawer::FloodDrawer(Object* object) 
  : HexagonDrawer(object)
{}

void Hexoworld::Hexagon::FloodDrawer::colorize_points()
{
  for (const auto& p : std::static_pointer_cast<FloodFrame>(base->frames[Flood])->waterPoints)
    Points::get_instance().set_point_color(p, base->world.floodColor, base);
}

Hexoworld::Hexagon::RoadDrawer::RoadDrawer(Object* object)
  : HexagonDrawer(object)
{}

void Hexoworld::Hexagon::RoadDrawer::colorize_points()
{
  for (auto& p : std::static_pointer_cast<RoadFrame>(base->frames[Road])->get_points())
    Points::get_instance().set_point_color(p, base->world.roadColor, base);
}
