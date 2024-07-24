#include <hexoworld/hexoworld.hpp>

Hexoworld::Hexagon::Hexagon(Hexoworld& hexoworld,
  Eigen::Vector3d center, Coord coord)
  : Object(hexoworld), coord(coord)
{
  frames[Usual] = std::make_shared<UsualFrame>(this, center);
  drawers[Usual] = std::make_shared<UsualDrawer>(this, Eigen::Vector4i(0, 0, 0, 0));
}

void Hexoworld::Hexagon::set_height(int32_t height)
{
  for (auto& [type, frame] : frames)
    std::static_pointer_cast<HexagonFrame>(frame)->set_height(height);
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::make_river(int32_t in, int32_t out)
{
  frames[River] = std::make_shared<RiversFrame>(this, in, out);
  drawers[River] = std::make_shared<RiverDrawer>(this);

  return std::static_pointer_cast<RiversFrame>(frames[River])->get_points();
}

void Hexoworld::Hexagon::make_flooding()
{
  frames [Flood] = std::make_shared<FloodFrame >(this, world.heightStep_ / 10);
  drawers[Flood] = std::make_shared<FloodDrawer>(this);
}

void Hexoworld::Hexagon::init_inventory()
{
  inventory.clear();
  if (frames.find(Road) != frames.end())
  {
    auto frame = std::static_pointer_cast<RoadFrame>(frames[Road]);

    for (auto i : frame->get_inner_fence())
      inventory.push_back(std::make_shared<Wall>(this,
        i.first, i.second,
        world.heightStep_ / 20, 0.01,
        Eigen::Vector4i(255, 255, 255, 255)));

    for (auto i : frame->get_outer_fence())
      inventory.push_back(std::make_shared<Wall>(this,
        i.first, i.second,
        world.heightStep_ / 10, 0.01,
        Eigen::Vector4i(255, 255, 255, 255)));
  }
}

void Hexoworld::Hexagon::make_road(std::vector<uint32_t> ind_roads)
{
  frames [Road] = std::make_shared<RoadFrame >(this, ind_roads);
  drawers[Road] = std::make_shared<RoadDrawer>(this);

  init_inventory();
}

void Hexoworld::Hexagon::make_road(uint32_t ind_road)
{
  auto frame = std::static_pointer_cast<RoadFrame>(frames.at(Road));
  if (!frame->isRoad[ind_road])
  {
    frame->add_road(ind_road);
    
    init_inventory();
  }
}

void Hexoworld::Hexagon::print_in_triList(std::vector<uint16_t>& TriList)
{
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);

  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();

  for (auto& inv : inventory)
    inv->print_in_triList(TriList);
}
