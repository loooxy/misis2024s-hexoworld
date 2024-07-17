#include <hexoworld/hexoworld.hpp>

Hexoworld::Hexagon::Hexagon(Hexoworld& hexoworld,
  Eigen::Vector3d center, Coord coord)
  : Object(hexoworld), coord(coord)
{
  frame = std::make_shared<UsualFrame>(this, center);
  drawer = std::make_shared<HexagonDrawer>(this, Eigen::Vector4i(0, 0, 0, 0));
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::make_river(int32_t in, int32_t out)
{
  {
    std::shared_ptr<MainData> mainData =
      (std::static_pointer_cast<HexagonFrame>(frame)->mainData);
    Eigen::Vector3d center = mainData->center;
    frame.reset();
    frame = std::make_shared<RiversFrame>(this, center, in, out, mainData);
  }

  {
    Eigen::Vector4i color = drawer->get_color();
    drawer.reset();
    drawer = std::make_shared<RiverDrawer>(this, color);
  }

  return std::static_pointer_cast<RiversFrame>(frame)->
    get_points();
}

void Hexoworld::Hexagon::print_in_triList(std::vector<uint16_t>& TriList)
{
  frame->print_in_triList(TriList);
  drawer->colorize_points();
}
