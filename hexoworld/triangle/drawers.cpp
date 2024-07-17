#include <hexoworld/hexoworld.hpp>

Hexoworld::Triangle::TriangleDrawer::TriangleDrawer(Object* object, Eigen::Vector4i color)
  : Drawer(object, color) {}

void Hexoworld::Triangle::TriangleDrawer::colorize_points() {
  Drawer::colorize_points();

  Triangle* tri = static_cast<Triangle*>(base);
  std::shared_ptr<TriangleFrame> frame = std::static_pointer_cast<TriangleFrame>(tri->frame);

  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color + (color_ - color) / 2,
      base);
    };

  col_id(frame->mainData->AId, tri->a_hex->drawer->get_color());
  col_id(frame->mainData->BId, tri->b_hex->drawer->get_color());
  col_id(frame->mainData->CId, tri->c_hex->drawer->get_color());
}
