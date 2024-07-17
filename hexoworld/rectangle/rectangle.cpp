#include <hexoworld/hexoworld.hpp>

Hexoworld::Rectangle::Rectangle(
  Hexoworld& hexoworld,
  Eigen::Vector3d a, Eigen::Vector3d b,
  Eigen::Vector3d c, Eigen::Vector3d d,
  std::vector<Eigen::Vector3d> ab_points,
  std::vector<Eigen::Vector3d> cd_points,
  const std::shared_ptr<Hexagon> ab_hex, const std::shared_ptr<Hexagon> cd_hex)
  : Object(hexoworld), ab_hex_(ab_hex), cd_hex_(cd_hex) {
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

  frame = std::make_shared<UsualFrame>(this, AId, BId, CId, DId, ab_ids, cd_ids);
  drawer = std::make_shared<RectangleDrawer>(this, Eigen::Vector4i(255, 0, 0, 255));
}

void Hexoworld::Rectangle::make_river()
{
  {
    std::shared_ptr<MainData> mainData =
      (std::static_pointer_cast<RectangleFrame>(frame)->mainData);
    frame.reset();
    frame = std::make_shared<RiverFrame>(this,
      mainData->AId,
      mainData->BId,
      mainData->CId,
      mainData->DId,
      mainData->ABIds,
      mainData->CDIds,
      mainData);
  }

  {
    Eigen::Vector4i color = drawer->get_color();
    drawer.reset();
    drawer = std::make_shared<RiverDrawer>(this, color);
  }
}

void Hexoworld::Rectangle::print_in_triList(std::vector<uint16_t>& TriList) {
  frame->print_in_triList(TriList);
  drawer->colorize_points();
}