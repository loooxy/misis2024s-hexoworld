#include <hexoworld/base_objects/hexagon/hexagon.hpp>
#include <hexoworld/wall/wall.hpp>
#include <hexoworld/cottage/cottage.hpp>
#include <random>
#include <cmath>


Hexoworld::Hexagon::Hexagon(Hexoworld& hexoworld,
  Eigen::Vector3d center, Coord coord)
  : Object(hexoworld), coord(coord)
{
  frames[Usual] = std::make_shared<UsualFrame>(this, center);
  drawers[Usual] = std::make_shared<UsualDrawer>(this, Eigen::Vector4i(0, 0, 0, 0));
  drawers[Usual]->colorize_points();
}

void Hexoworld::Hexagon::set_height(int32_t height)
{
  for (auto& [type, frame] : frames)
    std::static_pointer_cast<HexagonFrame>(frame)->set_height(height);

  init_inventory();
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::make_river(int32_t in, int32_t out)
{
  frames[River] = std::make_shared<RiversFrame>(this, in, out);
  drawers[River] = std::make_shared<RiverDrawer>(this);
  drawers[River]->colorize_points();

  return std::static_pointer_cast<RiversFrame>(frames[River])->get_points();
}

void Hexoworld::Hexagon::make_flooding()
{
  frames [Flood] = std::make_shared<FloodFrame >(this, world.heightStep_ / 10);
  drawers[Flood] = std::make_shared<FloodDrawer>(this);
  drawers[Flood]->colorize_points();
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

  if (is_numbers_show)
    init_number();
  
#ifdef BOUNDARY_WALLS
  init_boundary_walls();
#endif // BOUNDARY_WALLS

  if (mainData->dirFarm != -1)
  {
    auto center = Points::get_instance().get_point(mainData->centerId);
    std::vector<Eigen::Vector3d> polygonPoints;
    for (IdType i : mainData->polygonPointsId)
      polygonPoints.push_back(Points::get_instance().get_point(i));
    std::vector<std::vector<Eigen::Vector3d>> extraPoints;
    for (const auto& i : mainData->extraPointsId)
    {
      std::vector<Eigen::Vector3d> tmp;
      for (IdType j : i)
        tmp.push_back(Points::get_instance().get_point(j));
      extraPoints.push_back(tmp);
    }

    inventory.push_back(
      std::make_shared<Cottage>(
        this, 
        center, 
        extraPoints[mainData->dirFarm][1] - center
      )
    );

  }
}

void Hexoworld::Hexagon::init_number()
{
  //OneThreadController __otc__(&world, std::this_thread::get_id());
  auto center = Points::get_instance().get_point(mainData->centerId);
  std::vector<Eigen::Vector3d> polygonPoints;
  for (IdType i : mainData->polygonPointsId)
    polygonPoints.push_back(Points::get_instance().get_point(i));
  std::vector<std::vector<Eigen::Vector3d>> extraPoints;
  for (const auto& i : mainData->extraPointsId)
  {
    std::vector<Eigen::Vector3d> tmp;
    for (IdType j : i)
      tmp.push_back(Points::get_instance().get_point(j));
    extraPoints.push_back(tmp);
  }

  std::vector<Eigen::Vector3d> rows_points;
  {
    Eigen::Vector3d p1 = center + (polygonPoints[3] - center) / 3 * 2;
    Eigen::Vector3d p2 = center + (polygonPoints[3] - center) / 3;
    Eigen::Vector3d u = (extraPoints[4][1] - center) / 2;

    rows_points = {
      p1 + u, p2 + u,
      p1, p2,
      p1 - u, p2 - u
    };
  }
  std::vector<Eigen::Vector3d> cols_points;
  {
    Eigen::Vector3d p1 = center + (polygonPoints[0] - center) / 3;
    Eigen::Vector3d p2 = center + (polygonPoints[0] - center) / 3 * 2;
    Eigen::Vector3d u = (extraPoints[4][1] - center) / 2;

    cols_points = {
      p1 + u, p2 + u,
      p1, p2,
      p1 - u, p2 - u
    };
  }

  static const std::vector<std::vector<std::pair<uint32_t, uint32_t>>> sticks = {
    {
      {0, 1}, {1, 5}, {5, 4}, {0, 4}
},//0
    {
      {1, 5}
},//1
    {
      {0, 1}, {1, 3}, {3, 2}, {2, 4}, {4, 5}
},//2
    {
      {0, 1}, {2, 3}, {4, 5}, {1, 5}
},//3
    {
      {0, 2}, {2, 3}, {1, 5},
},//4
    {
      {1, 0}, {0, 2}, {2, 3}, {3, 5}, {5, 4}
},//5
    {
      {1, 0}, {0, 4}, {2, 3}, {3, 5}, {5, 4}
},//6
    {
      {0, 1}, {1, 4}
},//7
    {
      {0, 1}, {1, 5}, {0, 4}, {4, 5}, {2, 3}
},//8
    {
      {0, 1}, {2, 3}, {0, 2}, {1, 5}, {5, 4}
}
  };

  for (const auto& stick : sticks[coord.row % 10])
  {
    inventory.push_back(
      std::make_shared<Wall>(
        this,
        rows_points[stick.first],
        rows_points[stick.second],
        world.heightStep_ / 7,
        0.05,
        Eigen::Vector4i(0, 0, 0, 255)
      )
    );
  }
  
  for (const auto& stick : sticks[coord.col % 10])
  {
    inventory.push_back(
      std::make_shared<Wall>(
        this,
        cols_points[stick.first],
        cols_points[stick.second],
        world.heightStep_ / 7,
        0.05,
        Eigen::Vector4i(0, 0, 0, 255)
      )
    );
  }
}

void Hexoworld::Hexagon::init_boundary_walls()
{
  std::vector<Eigen::Vector3d> pointsId = frames[Usual]->get_points();
  for (uint32_t i = 0; i < pointsId.size() - 1; ++i)
    inventory.push_back(
      std::make_shared<Wall>(
        this,
        pointsId[i],
        pointsId[(i + 1) % (pointsId.size() - 1)],
        world.heightStep_ / 7,
        0.05,
        Eigen::Vector4i(0, 0, 0, 255)
      )
    );
}

void Hexoworld::Hexagon::make_road(std::vector<uint32_t> ind_roads)
{
  frames [Road] = std::make_shared<RoadFrame >(this, ind_roads);
  drawers[Road] = std::make_shared<RoadDrawer>(this);
  drawers[Road]->colorize_points();

  init_inventory();
}

void Hexoworld::Hexagon::make_road(uint32_t ind_road)
{
  if (frames.find(Road) == frames.end())
    make_road(std::vector<uint32_t>{ind_road});

  auto frame = std::static_pointer_cast<RoadFrame>(frames.at(Road));
  if (!frame->isRoad[ind_road])
  {
    frame->add_road(ind_road);
    
    init_inventory();
  }
}

void Hexoworld::Hexagon::del_road()
{
  if (frames.find(Road) != frames.end())
  {
    frames.erase(Road);
    drawers.erase(Road);
    init_inventory();
  }
}

void Hexoworld::Hexagon::del_road(uint32_t ind_road)
{
  auto frame = std::static_pointer_cast<RoadFrame>(frames.at(Road));
  if (frame->isRoad[ind_road])
  {
    frame->del_road(ind_road);

    init_inventory();
  }
}

void Hexoworld::Hexagon::make_farm()
{
  std::random_device rd;
  mainData->dirFarm = rd() % 6;
  //std::static_pointer_cast<UsualDrawer>(drawers[Usual])->set_color(Eigen::Vector4i(241, 215, 129, 255));
  init_inventory();
}

void Hexoworld::Hexagon::del_farm()
{
  mainData->dirFarm = -1;
  init_inventory();
}

void Hexoworld::Hexagon::show_numbers()
{
  is_numbers_show = true;
  init_inventory();
}

void Hexoworld::Hexagon::hide_numbers()
{
  is_numbers_show = false;
  init_inventory();
}


void Hexoworld::Hexagon::print_in_triList(std::vector<uint32_t>& TriList)
{
  for (auto& [type, frame] : frames)
    frame->print_in_triList(TriList);

  for (auto& inv : inventory)
    inv->print_in_triList(TriList);
}

void Hexoworld::Hexagon::colorize_points()
{
  for (auto& [type, drawer] : drawers)
    drawer->colorize_points();

  for (auto& inv : inventory)
    inv->colorize_points();
}
