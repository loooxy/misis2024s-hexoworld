#include <hexoworld/hexoworld.hpp>


Hexoworld::Rectangle::RectangleDrawer::RectangleDrawer(Object* object, Eigen::Vector4i color)
  : Drawer(object, color) {}

void Hexoworld::Rectangle::RectangleDrawer::colorize_points() {
  Drawer::colorize_points();

  Rectangle* rect = static_cast<Rectangle*>(base);
  std::shared_ptr<RectangleFrame> frame = std::static_pointer_cast<RectangleFrame>(rect->frame);

  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color + (color_ - color) / 3,
      base);
    };

  col_id(frame->mainData->AId, rect->ab_hex_->drawer->get_color());
  for (int i = 0; i < frame->mainData->ABIds.size(); ++i)
    col_id(frame->mainData->ABIds[i], rect->ab_hex_->drawer->get_color());
  col_id(frame->mainData->BId, rect->ab_hex_->drawer->get_color());

  col_id(frame->mainData->CId, rect->cd_hex_->drawer->get_color());
  for (int i = 0; i < frame->mainData->CDIds.size(); ++i)
    col_id(frame->mainData->CDIds[i], rect->cd_hex_->drawer->get_color());
  col_id(frame->mainData->DId, rect->cd_hex_->drawer->get_color());
}

void Hexoworld::Rectangle::RiverDrawer::colorize_points() {
  RectangleDrawer::colorize_points();

  Rectangle* rect = static_cast<Rectangle*>(base);
  std::shared_ptr<RiverFrame> frame = std::static_pointer_cast<RiverFrame>(rect->frame);

  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color,
      base);
    };

  for (int i = 0; i < frame->waterPoints.size(); ++i)
    col_id(Points::get_instance().get_id_point(frame->waterPoints[i], rect), base->world.riverColor);
}