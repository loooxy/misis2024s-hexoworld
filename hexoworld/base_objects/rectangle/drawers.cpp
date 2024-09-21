#include <hexoworld/base_objects/rectangle/rectangle.hpp>
#include <hexoworld/base_objects/hexagon/hexagon.hpp>
#include <random>

Hexoworld::Rectangle::RectangleDrawer::RectangleDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Rectangle::UsualDrawer::UsualDrawer(Object* object)
  : RectangleDrawer(object)
{}

void Hexoworld::Rectangle::UsualDrawer::colorize_points() {
  Rectangle* rect = static_cast<Rectangle*>(base);
  
  Eigen::Vector4i ab_color = 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->ab_hex_->drawers.at(Usual))->get_color();
  Eigen::Vector4i cd_color = 
    std::static_pointer_cast<Hexagon::UsualDrawer>(rect->cd_hex_->drawers.at(Usual))->get_color();

  auto col_id = [this, ab_color, cd_color](IdType id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(
      id,
      color + ((ab_color + cd_color) / 2 - color) / 5);
    };

  std::vector<Eigen::Vector3d> ab = { Points::get_instance().get_point(rect->mainData->AId) };
  for (IdType id : rect->mainData->ABIds)
    ab.push_back(Points::get_instance().get_point(id));
  ab.push_back(Points::get_instance().get_point(rect->mainData->BId));
  
  std::vector<Eigen::Vector3d> cd = { Points::get_instance().get_point(rect->mainData->CId) };
  for (IdType id : rect->mainData->CDIds)
    cd.push_back(Points::get_instance().get_point(id));
  cd.push_back(Points::get_instance().get_point(rect->mainData->DId));

  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < rect->mainData->stairs[i].size(); ++j)
    {
      Eigen::Vector3d p = Points::get_instance().get_point(rect->mainData->stairs[i][j].first);

      double ab_len = std::numeric_limits<double>::max();
      for (const Eigen::Vector3d& p1 : ab)
        ab_len = std::min(ab_len, (p - p1).norm());

      double cd_len = std::numeric_limits<double>::max();
      for (const Eigen::Vector3d& p1 : cd)
        cd_len = std::min(cd_len, (p - p1).norm());

      ab_len = pow(ab_len, 3) * 100;
      cd_len = pow(cd_len, 3) * 100;

      Eigen::Vector4i color = 
        (ab_color * int(round(cd_len)) +
         cd_color * int(round(ab_len))) /
        (int(round(ab_len)) + int(round(cd_len)));

      col_id(rect->mainData->stairs[i][j].first, color);
      col_id(rect->mainData->stairs[i][j].second, color);
    }
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

  auto col_id = [this](IdType id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(id, color);
    };

  for (int i = 0; i < frame->waterPointsId.size(); ++i)
    col_id(frame->waterPointsId[i], special_color_river);
  for (int i = 0; i < frame->shorePointsId.size(); ++i)
    col_id(frame->shorePointsId[i], special_color_river);
}

void Hexoworld::Rectangle::RiverDrawer::set_new_special_color_river(Eigen::Vector4i new_color)
{
  special_color_river = new_color;
  for (const auto& point : std::static_pointer_cast<RiverFrame>(base->frames[River])->waterPointsId)
    Points::get_instance().set_point_color(point, special_color_river);
}

Hexoworld::Rectangle::RoadDrawer::RoadDrawer(Object* object)
  :RectangleDrawer(object){}

void Hexoworld::Rectangle::RoadDrawer::colorize_points()
{
  Rectangle* rect = static_cast<Rectangle*>(base);
  std::shared_ptr<RoadFrame> frame = std::static_pointer_cast<RoadFrame>(rect->frames[Road]);

  auto col_id = [this](IdType id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(id, color);
    };

  for (int i = 0; i < frame->middlePointsId.size(); ++i)
    col_id(frame->middlePointsId[i], base->world.roadColor);
  for (int i = 0; i < frame->fencePointsId.size(); ++i)
    col_id(frame->fencePointsId[i], base->world.roadColor);
}
