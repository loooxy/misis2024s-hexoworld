#include <hexoworld/hexoworld.hpp>

Hexoworld::Triangle::TriangleDrawer::TriangleDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Triangle::UsualDrawer::UsualDrawer(Object* object, Eigen::Vector4i color)
  : TriangleDrawer(object), color_(color)
{}

void Hexoworld::Triangle::UsualDrawer::colorize_points() {
  Triangle* tri = static_cast<Triangle*>(base);
  
  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color + (color_ - color) / 2,
      base);
    };

  col_id(tri->mainData->AId, std::static_pointer_cast<Hexagon::UsualDrawer>(tri->a_hex->drawers.at(Usual))
    ->get_color());
  col_id(tri->mainData->BId, std::static_pointer_cast<Hexagon::UsualDrawer>(tri->b_hex->drawers.at(Usual))
    ->get_color());
  col_id(tri->mainData->CId, std::static_pointer_cast<Hexagon::UsualDrawer>(tri->c_hex->drawers.at(Usual))
    ->get_color());
}
