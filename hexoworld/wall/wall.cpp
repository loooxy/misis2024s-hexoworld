#include <hexoworld/wall/wall.hpp>

Hexoworld::Wall::Wall(Object* base, 
  Eigen::Vector3d start, Eigen::Vector3d end, 
  double height, double width, 
  Eigen::Vector4i color)
  : FixedInventory(base)
{
  frames[Usual] = std::make_shared<UsualFrame>(this, 
    start, end, height, width, 
    base->world.heightDirection_);

  drawers[Usual] = std::make_shared<UsualDrawer>(this, color);
}

Hexoworld::Wall::Wall(Object* base, 
  Eigen::Vector3d start, Eigen::Vector3d end, 
  double height, double width, 
  Eigen::Vector3d heightDirection,
  Eigen::Vector4i color)
  : FixedInventory(base)
{
  frames[Usual] = std::make_shared<UsualFrame>(this, 
    start, end, height, width, 
    heightDirection);

  drawers[Usual] = std::make_shared<UsualDrawer>(this, color);
}

std::vector<Eigen::Vector3d> Hexoworld::Wall::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& [type, frame] : frames)
    for (const auto& p : frame->get_points())
      answer.push_back(p);
  return answer;
}

void Hexoworld::Wall::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);
}

void Hexoworld::Wall::colorize_points()
{
  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();
}
