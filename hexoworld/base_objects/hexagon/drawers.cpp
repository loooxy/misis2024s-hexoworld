#include <hexoworld/base_objects/hexagon/hexagon.hpp>
#include <random>
#include <cmath>

Hexoworld::Hexagon::HexagonDrawer::HexagonDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Hexagon::UsualDrawer::UsualDrawer(Object* base, Eigen::Vector4i color)
  : HexagonDrawer(base), color_(color)
{}

void Hexoworld::Hexagon::UsualDrawer::colorize_points()
{
  auto frame = std::static_pointer_cast<UsualFrame>(base->frames[Usual]);

  for (const auto& point : frame->get_pointsId())
    Points::get_instance().set_point_color(point, color_);
}

Hexoworld::Hexagon::RiverDrawer::RiverDrawer(Object* object)
  : HexagonDrawer(object) 
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-20, 20);
  auto make_new_component = [&gen, &dis](int32_t component) -> int {
    component += dis(gen);
    return std::min(255, std::max(0, component));
    };

  special_color_river = Eigen::Vector4i(
    make_new_component(base->world.riverColor.x()),
    make_new_component(base->world.riverColor.y()),
    make_new_component(base->world.riverColor.z()),
    make_new_component(base->world.riverColor.w())
  );
}
void Hexoworld::Hexagon::RiverDrawer::colorize_points()
{
  auto frame = std::static_pointer_cast<RiversFrame>(base->frames[River]);

  for (const auto& point : frame->get_water_pointsId())
    Points::get_instance().set_point_color(point, special_color_river);

  for (const auto& point : frame->get_shore_pointsId())
    Points::get_instance().set_point_color(point, base->world.riverColor);
}

void Hexoworld::Hexagon::RiverDrawer::set_new_special_color_river(Eigen::Vector4i new_color)
{
  special_color_river = new_color;

  auto frame = std::static_pointer_cast<RiversFrame>(base->frames[River]);

  for (const auto& point : frame->get_water_pointsId())
    Points::get_instance().set_point_color(point, special_color_river);
}

Hexoworld::Hexagon::FloodDrawer::FloodDrawer(Object* object) 
  : HexagonDrawer(object)
{}

void Hexoworld::Hexagon::FloodDrawer::colorize_points()
{
  auto frame = std::static_pointer_cast<FloodFrame>(base->frames[Flood]);

  for (const auto& p : frame->get_pointsId())
    Points::get_instance().set_point_color(p, base->world.floodColor);
}

Hexoworld::Hexagon::RoadDrawer::RoadDrawer(Object* object)
  : HexagonDrawer(object)
{}

void Hexoworld::Hexagon::RoadDrawer::colorize_points()
{
  auto frame = std::static_pointer_cast<RoadFrame>(base->frames[Road]);

  for (auto& p : frame->get_pointsId())
    Points::get_instance().set_point_color(p, base->world.roadColor);
}
