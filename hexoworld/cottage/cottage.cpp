#include <hexoworld/cottage/cottage.hpp>

Hexoworld::Cottage::Cottage(Object* base,
  Eigen::Vector3d center, Eigen::Vector3d mainDirection)
  : FixedInventory(base)
{
  mainDirection.normalize();

  const auto& wallsFrame = std::make_shared<WallsFrame>(this, center, mainDirection);
  frames[Walls] = wallsFrame;
  drawers[Walls] = std::make_shared<WallsDrawer>(this);

  frames[Roof] = std::make_shared<RoofFrame>(this,
    wallsFrame->get_points_topRect(),
    mainDirection);
  drawers[Roof] = std::make_shared<RoofDrawer>(this);
}

std::vector<Eigen::Vector3d> Hexoworld::Cottage::get_points() const {
  std::set<Eigen::Vector3d, EigenVector3dComp> st_answer;
  for (const auto& [type, frame] : frames)
  {
    for (const auto& point : frame->get_points())
      st_answer.insert(point);
  }

  std::vector<Eigen::Vector3d> answer;
  for (const auto& point : st_answer)
    answer.push_back(point);

  return answer;
}

void Hexoworld::Cottage::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (const auto& [type, frame] : frames)
    frame->print_in_triList(TriList);
}

void Hexoworld::Cottage::colorize_points()
{
  for (const auto& [type, drawer] : drawers)
    drawer->colorize_points();
}
