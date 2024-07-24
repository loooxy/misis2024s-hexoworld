#include <hexoworld/hexoworld.hpp>

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
  drawers[Usual] = std::make_shared<UsualDrawer>(this, Eigen::Vector4i(255, 0, 0, 255));
}

void Hexoworld::Triangle::print_in_triList(std::vector<uint16_t>& TriList) {
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);

  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();
}
