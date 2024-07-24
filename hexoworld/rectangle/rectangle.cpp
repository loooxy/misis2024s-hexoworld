#include "rectangle.hpp"
#include "rectangle.hpp"
#include <hexoworld/hexoworld.hpp>

Hexoworld::Rectangle::Rectangle(
  Hexoworld& hexoworld,
  Eigen::Vector3d a, Eigen::Vector3d b,
  Eigen::Vector3d c, Eigen::Vector3d d,
  std::vector<Eigen::Vector3d> ab_points,
  std::vector<Eigen::Vector3d> cd_points,
  const std::shared_ptr<Hexagon> ab_hex, const std::shared_ptr<Hexagon> cd_hex)
  : Object(hexoworld), ab_hex_(ab_hex), cd_hex_(cd_hex) 
{
  uint32_t AId = Points::get_instance().get_id_point(a, this);
  uint32_t BId = Points::get_instance().get_id_point(b, this);
  uint32_t CId = Points::get_instance().get_id_point(c, this);
  uint32_t DId = Points::get_instance().get_id_point(d, this);
  std::vector<uint32_t> ab_ids;
  for (Eigen::Vector3d& point : ab_points)
    ab_ids.push_back(Points::get_instance().get_id_point(point, this));
  std::vector<uint32_t> cd_ids;
  for (Eigen::Vector3d& point : cd_points)
    cd_ids.push_back(Points::get_instance().get_id_point(point, this));

  frames [Usual] = std::make_shared<UsualFrame >(this, AId, BId, CId, DId, ab_ids, cd_ids);
  drawers[Usual] = std::make_shared<UsualDrawer>(this, Eigen::Vector4i(255, 0, 0, 255));
}

void Hexoworld::Rectangle::update()
{
  frames[Usual] = std::make_shared<UsualFrame >(this, 
    mainData->AId, 
    mainData->BId, 
    mainData->CId, 
    mainData->DId, 
    mainData->ABIds, 
    mainData->CDIds);
  drawers[Usual] = std::make_shared<UsualDrawer>(this, 
    std::static_pointer_cast<UsualDrawer>(drawers.at(Usual))->get_color());
}

void Hexoworld::Rectangle::make_river()
{
  frames [River] = std::make_shared<RiverFrame >(this);
  drawers[River] = std::make_shared<RiverDrawer>(this);
}

void Hexoworld::Rectangle::make_road()
{
  frames [Road] = std::make_shared<RoadFrame >(this);
  drawers[Road] = std::make_shared<RoadDrawer>(this);

  auto frame = std::static_pointer_cast<RoadFrame>(frames.at(Road));

  inventory.push_back(std::make_shared<Wall>(this,
    frame->middlePoints[0], 
    frame->middlePoints[1],
    world.heightStep_ / 20, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));

  inventory.push_back(std::make_shared<Wall>(this,
    frame->fencePoints[0], frame->fencePoints[1],
    world.heightStep_ / 10, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));
  inventory.push_back(std::make_shared<Wall>(this,
    frame->fencePoints[2], frame->fencePoints[3],
    world.heightStep_ / 10, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));
}

void Hexoworld::Rectangle::print_in_triList(std::vector<uint16_t>& TriList) {
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);

  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();

  for (auto& inv : inventory)
    inv->print_in_triList(TriList);
}