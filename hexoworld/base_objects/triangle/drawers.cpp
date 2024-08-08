#include <hexoworld/base_objects/triangle/triangle.hpp>
#include <hexoworld/base_objects/hexagon/hexagon.hpp>

Hexoworld::Triangle::TriangleDrawer::TriangleDrawer(Object* object)
  : Drawer(object) {}

Hexoworld::Triangle::UsualDrawer::UsualDrawer(Object* object)
  : TriangleDrawer(object)
{}

void Hexoworld::Triangle::UsualDrawer::colorize_points() {
  Triangle* tri = static_cast<Triangle*>(base);
  
  auto col_id = [this](uint32_t id, Eigen::Vector4i color) {
    Points::get_instance().set_point_color(id, color);
    };

  Eigen::Vector4i a_color = std::static_pointer_cast<Hexagon::UsualDrawer>(tri->a_hex->drawers.at(Usual))
    ->get_color();
  Eigen::Vector4i b_color = std::static_pointer_cast<Hexagon::UsualDrawer>(tri->b_hex->drawers.at(Usual))
    ->get_color();
  Eigen::Vector4i c_color = std::static_pointer_cast<Hexagon::UsualDrawer>(tri->c_hex->drawers.at(Usual))
    ->get_color();

  Eigen::Vector3d a = Points::get_instance().get_point(tri->mainData->AId);
  Eigen::Vector3d b = Points::get_instance().get_point(tri->mainData->BId);
  Eigen::Vector3d c = Points::get_instance().get_point(tri->mainData->CId);

  auto eqw = [this](Eigen::Vector3d a, Eigen::Vector3d b) {
    Eigen::Vector3d diff = a - b;
    return(
      abs(diff.x()) < PRECISION_DBL_CALC &&
      abs(diff.y()) < PRECISION_DBL_CALC &&
      abs(diff.z()) < PRECISION_DBL_CALC);
    };

  auto cos_line = [this, eqw](Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c) {
    double cos = (c - a).normalized().dot((b - a).normalized());
    return cos;
    };
  
  auto get_color = [this, eqw](
    Eigen::Vector3d a, Eigen::Vector4i a_color, const std::shared_ptr<Hexagon> a_hex,
    Eigen::Vector3d b, Eigen::Vector4i b_color, const std::shared_ptr<Hexagon> b_hex,
    Eigen::Vector3d p)
    -> Eigen::Vector4i
    {
      double a_len = (p - a).norm();
      double b_len = (p - b).norm();

      a_len = pow(a_len, 3) * 100;
      b_len = pow(b_len, 3) * 100;

      Eigen::Vector4i color = (
        a_color * int(round(b_len)) +
        b_color * int(round(a_len))) /
        (int(round(a_len)) +
          int(round(b_len)));

      Eigen::Vector4i dop_color = (
        std::static_pointer_cast<Hexagon::UsualDrawer>
        (a_hex->drawers.at(Usual))->get_color() +
        std::static_pointer_cast<Hexagon::UsualDrawer>
        (b_hex->drawers.at(Usual))->get_color()) / 2;

      return color + (dop_color - color) / 5;
    };

  for (auto& pId : std::static_pointer_cast<UsualFrame>(tri->frames[Usual])->get_pointsId()) 
  {
    Eigen::Vector3d p = Points::get_instance().get_point(pId);
    if (!eqw(p, a) && !eqw(p, b) && !eqw(p, c))
    {
      double ab_cos = cos_line(a, b, p);
      double ac_cos = cos_line(a, c, p);
      double bc_cos = cos_line(b, c, p);
      double mx_cos = std::max(std::max(ab_cos, ac_cos), bc_cos);

      Eigen::Vector4i color;
      if (ab_cos == mx_cos)
      {
        color = get_color(
          a, a_color, static_cast<Triangle*>(base)->a_hex,
          b, b_color, static_cast<Triangle*>(base)->b_hex,
          p);
      }
      else if (ac_cos == mx_cos)
      {
        color = get_color(
          a, a_color, static_cast<Triangle*>(base)->a_hex,
          c, c_color, static_cast<Triangle*>(base)->c_hex,
          p);
      }
      else
      {
        color = get_color(
          b, b_color, static_cast<Triangle*>(base)->b_hex,
          c, c_color, static_cast<Triangle*>(base)->c_hex,
          p);
      }

      Points::get_instance().set_point_color(pId, color);
    }
  }

  col_id(tri->mainData->AId, 
    a_color + ((a_color == b_color ? (a_color + c_color) / 2 : (a_color + b_color)/2) - a_color) / 5);
  col_id(tri->mainData->BId, 
    b_color + ((b_color == a_color ? (b_color + c_color) / 2 : (b_color + a_color) / 2) - b_color) / 5);
  col_id(tri->mainData->CId, 
    c_color + ((c_color == a_color ? (c_color + b_color) / 2 : (c_color + a_color) / 2) - c_color) / 5);
}
