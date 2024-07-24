#include <hexoworld/hexoworld.hpp>
#include <random>

Hexoworld::Rectangle::RectangleDrawer::RectangleDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Rectangle::UsualDrawer::UsualDrawer(Object* object, Eigen::Vector4i color)
  : RectangleDrawer(object), color_(color)
{}

void Hexoworld::Rectangle::UsualDrawer::colorize_points() {
  Rectangle* rect = static_cast<Rectangle*>(base);
  
  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color + (color_ - color) / 3,
      base);
    };

  col_id(rect->mainData->AId, 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->ab_hex_->drawers.at(Usual))->get_color());

  for (int i = 0; i < rect->mainData->ABIds.size(); ++i)
    col_id(rect->mainData->ABIds[i], 
      std::static_pointer_cast<Hexagon::UsualDrawer>(rect->ab_hex_->drawers.at(Usual))->get_color());
  col_id(rect->mainData->BId, 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->ab_hex_->drawers.at(Usual))->get_color());

  col_id(rect->mainData->CId, 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->cd_hex_->drawers.at(Usual))->get_color());
  for (int i = 0; i < rect->mainData->CDIds.size(); ++i)
    col_id(rect->mainData->CDIds[i], 
      std::static_pointer_cast<Hexagon::UsualDrawer>(rect->cd_hex_->drawers.at(Usual))->get_color());
  col_id(rect->mainData->DId, 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->cd_hex_->drawers.at(Usual))->get_color());
}

Hexoworld::Rectangle::RiverDrawer::RiverDrawer(Object* object)
  : RectangleDrawer(object)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-20, 20);
  auto make_new_component = [&gen, &dis](int32_t component) {
    component += dis(gen);
    return std::min(255, std::max(0, component));
    };

  special_color_river = Eigen::Vector4i(
    make_new_component(base->world.riverColor.x()),
    make_new_component(base->world.riverColor.y()),
    make_new_component(base->world.riverColor.z()),
    255
  );
}

void Hexoworld::Rectangle::RiverDrawer::colorize_points() {
  Rectangle* rect = static_cast<Rectangle*>(base);
  std::shared_ptr<RiverFrame> frame = std::static_pointer_cast<RiverFrame>(rect->frames[River]);

  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color,
      base);
    };

  for (int i = 0; i < frame->waterPoints.size(); ++i)
    col_id(Points::get_instance().get_id_point(frame->waterPoints[i], rect), special_color_river);
  for (int i = 0; i < frame->shorePoints.size(); ++i)
    col_id(Points::get_instance().get_id_point(frame->shorePoints[i], rect), special_color_river);
}

void Hexoworld::Rectangle::RiverDrawer::set_new_special_color_river(Eigen::Vector4i new_color)
{
  special_color_river = new_color;
  for (const auto& point : std::static_pointer_cast<RiverFrame>(base->frames[River])->waterPoints)
    Points::get_instance().set_point_color(point, special_color_river, base);
}

Hexoworld::Rectangle::RoadDrawer::RoadDrawer(Object* object)
  :RectangleDrawer(object){}

void Hexoworld::Rectangle::RoadDrawer::colorize_points()
{
  Rectangle* rect = static_cast<Rectangle*>(base);
  std::shared_ptr<RoadFrame> frame = std::static_pointer_cast<RoadFrame>(rect->frames[Road]);

  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      Points::get_instance().get_point(id),
      color,
      base);
    };

  for (int i = 0; i < frame->middlePoints.size(); ++i)
    col_id(Points::get_instance().get_id_point(frame->middlePoints[i], rect), base->world.roadColor);
  for (int i = 0; i < frame->fencePoints.size(); ++i)
    col_id(Points::get_instance().get_id_point(frame->fencePoints[i], rect), base->world.roadColor);
}
