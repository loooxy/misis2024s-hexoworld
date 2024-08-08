#include <hexoworld/base_objects/rectangle/rectangle.hpp>
#include <hexoworld/wall/wall.hpp>

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
  drawers[Usual] = std::make_shared<UsualDrawer>(this);
}

void Hexoworld::Rectangle::update()
{
  inventory.clear();

  uint32_t AId = mainData->AId;
  uint32_t BId = mainData->BId;
  uint32_t CId = mainData->CId;
  uint32_t DId = mainData->DId;
  std::vector<uint32_t> ABIds = mainData->ABIds;
  std::vector<uint32_t> CDIds = mainData->CDIds;
  mainData.reset();

  frames[Usual] = std::make_shared<UsualFrame >(this, 
    AId, 
    BId, 
    CId, 
    DId, 
    ABIds, 
    CDIds);
  drawers[Usual] = std::make_shared<UsualDrawer>(this);

  if (frames.find(River) != frames.end())
  {
    frames[River] = std::make_shared<RiverFrame>(this);
    drawers[River] = std::make_shared<RiverDrawer>(this);
  }

  if (frames.find(Road) != frames.end())
  {
    frames[Road] = std::make_shared<RoadFrame>(this);
    drawers[Road] = std::make_shared<RoadDrawer>(this);

    auto frame = std::static_pointer_cast<RoadFrame>(frames.at(Road));

    inventory.push_back(std::make_shared<Wall>(this,
      Points::get_instance().get_point(frame->middlePointsId[0]),
      Points::get_instance().get_point(frame->middlePointsId[1]),
      world.heightStep_ / 20, 0.01,
      Eigen::Vector4i(255, 255, 255, 255)));

    inventory.push_back(std::make_shared<Wall>(this,
      Points::get_instance().get_point(frame->fencePointsId[0]), 
      Points::get_instance().get_point(frame->fencePointsId[1]),
      world.heightStep_ / 10, 0.01,
      Eigen::Vector4i(255, 255, 255, 255)));
    inventory.push_back(std::make_shared<Wall>(this,
      Points::get_instance().get_point(frame->fencePointsId[2]),
      Points::get_instance().get_point(frame->fencePointsId[3]),
      world.heightStep_ / 10, 0.01,
      Eigen::Vector4i(255, 255, 255, 255)));
  }
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
    Points::get_instance().get_point(frame->middlePointsId[0]),
    Points::get_instance().get_point(frame->middlePointsId[1]),
    world.heightStep_ / 20, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));

  inventory.push_back(std::make_shared<Wall>(this,
    Points::get_instance().get_point(frame->fencePointsId[0]),
    Points::get_instance().get_point(frame->fencePointsId[1]),
    world.heightStep_ / 10, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));
  inventory.push_back(std::make_shared<Wall>(this,
    Points::get_instance().get_point(frame->fencePointsId[2]),
    Points::get_instance().get_point(frame->fencePointsId[3]),
    world.heightStep_ / 10, 0.01,
    Eigen::Vector4i(255, 255, 255, 255)));
}

void Hexoworld::Rectangle::print_in_triList(std::vector<uint32_t>& TriList) {
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);

  for (auto& inv : inventory)
    inv->print_in_triList(TriList);
}

void Hexoworld::Rectangle::colorize_points()
{
  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();

  for (auto& inv : inventory)
    inv->colorize_points();
}
