#include <hexoworld/base_objects/triangle/triangle.hpp>

Hexoworld::Triangle::Triangle(Hexoworld& hexoworld,
  Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  const std::shared_ptr<Hexagon> a_hex,
  const std::shared_ptr<Hexagon> b_hex,
  const std::shared_ptr<Hexagon> c_hex)
  : Object(hexoworld), a_hex(a_hex), b_hex(b_hex), c_hex(c_hex)
{
  std::vector<uint32_t> ids = {
    Points::get_instance().get_id_point(a, this) ,
    Points::get_instance().get_id_point(b, this) ,
    Points::get_instance().get_id_point(c, this) };

  frames [Usual] = std::make_shared<UsualFrame >(this, ids[0], ids[1], ids[2]);
  drawers[Usual] = std::make_shared<UsualDrawer>(this);
}

void Hexoworld::Triangle::update()
{
  uint32_t aid = mainData->AId;
  uint32_t bid = mainData->BId;
  uint32_t cid = mainData->CId;

  mainData.reset();

  frames[Usual] = std::make_shared<UsualFrame>(this, aid, bid, cid);
  drawers[Usual] = std::make_shared<UsualDrawer>(this);
}

void Hexoworld::Triangle::print_in_triList(std::vector<uint32_t>& TriList) {
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);
}

void Hexoworld::Triangle::colorize_points()
{
  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();
}
