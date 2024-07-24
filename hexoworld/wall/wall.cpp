#include "wall.hpp"
#include <hexoworld/hexoworld.hpp>

Hexoworld::Wall::Wall(Object* base, 
  Eigen::Vector3d start, Eigen::Vector3d end, 
  double height, double width, 
  Eigen::Vector4i color)
  : FixedInventory(base)
{
  frames[Usual] = std::make_shared<UsualFrame>(this, start, end, height, width);
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

void Hexoworld::Wall::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);
  
  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();
}
