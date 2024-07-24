#include "hexagon.hpp"
#include "hexagon.hpp"
#include <hexoworld/hexoworld.hpp>
#include <random>
#include <cmath>

Hexoworld::Hexagon::HexagonFrame::HexagonFrame(Object* base)
  : Frame(base)
{}

//struct UsualType-----------------------------------------

Hexoworld::Hexagon::UsualFrame::UsualFrame(Object* base, Eigen::Vector3d center)
  : HexagonFrame(base)
{
  if (static_cast<Hexagon*>(base)->mainData == nullptr)
  {
    std::shared_ptr<MainData> mainData =
      (static_cast<Hexagon*>(base)->mainData = std::make_shared<MainData>());

    std::random_device rd;
    mainData->gen_init = rd();

    Eigen::Vector3d dop = base->world.dop_height(Usual);
    mainData->center = center + dop;

    const Eigen::Vector3d& pointDirection = base->world.rowDirection_;
    const Eigen::Vector3d& floatDirection = base->world.colDirection_;

    mainData->polygonPoints.resize(6);
    {
      mainData->polygonPoints[0] = mainData->center + (pointDirection * base->world.size_);
      mainData->polygonPoints[1] =
        mainData->center + (pointDirection * (0.5 * base->world.size_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->polygonPoints[2] =
        mainData->center - (pointDirection * (0.5 * base->world.size_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->polygonPoints[3] = mainData->center - (pointDirection * base->world.size_);
      mainData->polygonPoints[4] =
        mainData->center - (pointDirection * (0.5 * base->world.size_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->polygonPoints[5] =
        mainData->center + (pointDirection * (0.5 * base->world.size_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
    }
    mainData->extraPoints.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      Eigen::Vector3d a = mainData->polygonPoints[i];
      Eigen::Vector3d b = mainData->polygonPoints[(i + 1) % 6];
      Eigen::Vector3d v = (b - a) / 4;
      mainData->extraPoints[i].resize(3);
      for (int j = 0; j < 3; ++j)
        mainData->extraPoints[i][j] = a + v * (j + 1);
    }

    /*{
      std::mt19937 gen(mainData->gen_init);
      std::uniform_real_distribution<> dis(-0.2, 0.2);
      double height_move = dis(gen);
      for (Eigen::Vector3d& point : mainData->polygonPoints)
      {
        point +=
          base->world.rowDirection_ * dis(gen) +
          base->world.colDirection_ * dis(gen) +
          base->world.heightDirection_ * height_move;
      }
      for (uint32_t i = 0; i < 6; ++i)
      {
        for (uint32_t j = 0; j <= 2; ++j)
        {
          Eigen::Vector3d& point = mainData->extraPoints[i][j];
          point +=
            base->world.rowDirection_ * dis(gen) +
            base->world.colDirection_ * dis(gen) +
            base->world.heightDirection_ * height_move;
        }
      }
      mainData->center +=
        base->world.rowDirection_ * dis(gen) +
        base->world.colDirection_ * dis(gen) +
        base->world.heightDirection_ * height_move;
    }*/

    mainData->polygonPointsId.resize(6);
    for (int i = 0; i < 6; ++i)
      mainData->polygonPointsId[i] =
      Points::get_instance().get_id_point(
        mainData->polygonPoints[i], base);

    mainData->extraPointsId.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      mainData->extraPointsId[i].resize(
        mainData->extraPoints[i].size());
      for (int j = 0; j < mainData->extraPoints[i].size(); ++j)
        mainData->extraPointsId[i][j] =
        Points::get_instance().get_id_point(
          mainData->extraPoints[i][j], base);
    }
  }
}

void Hexoworld::Hexagon::UsualFrame::set_height(int32_t height)
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;

  for (int i = 0; i < 6; ++i)
  {
    mainData->polygonPoints[i] = Points::get_instance().get_point(mainData->polygonPointsId[i]);
    base->world.set_new_height_to_point(mainData->polygonPoints[i], height, Usual);
    Points::get_instance().update_point(mainData->polygonPointsId[i], mainData->polygonPoints[i]);

    for (int j = 0; j < 3; ++j)
    {
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);
      base->world.set_new_height_to_point(mainData->extraPoints[i][j], height, Usual);
      Points::get_instance().update_point(mainData->extraPointsId[i][j], mainData->extraPoints[i][j]);
    }
  }
  base->world.set_new_height_to_point(mainData->center, height, Usual);
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::UsualFrame::get_points() const
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;

  std::vector<Eigen::Vector3d> answer;
  for (int i = 0; i < 6; ++i)
  {
    mainData->polygonPoints[i] = Points::get_instance().get_point(mainData->polygonPointsId[i]);
    answer.push_back(mainData->polygonPoints[i]);

    for (int j = 0; j < 3; ++j)
    {
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);
      answer.push_back(mainData->extraPoints[i][j]);
    }
  }
  answer.push_back(mainData->center);
  return answer;
}

void Hexoworld::Hexagon::UsualFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;

  for (int i = 0; i < 6; ++i)
    mainData->polygonPoints[i] = Points::get_instance().get_point(mainData->polygonPointsId[i]);
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < mainData->extraPoints[i].size(); ++j)
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);

  std::vector<Eigen::Vector3d> points;
  for (int i = 0; i < 6; ++i)
  {
    points.push_back(mainData->polygonPoints[i]);
    for (int j = 0; j < mainData->extraPoints[i].size(); ++j)
      points.push_back(mainData->extraPoints[i][j]);
  }

  for (int i = 0; i < points.size(); ++i)
    printTri(mainData->center, points[i], points[(i + 1) % points.size()], TriList, base);
}

//struct RiversType----------------------------------------

Hexoworld::Hexagon::RiversFrame::RiversFrame(Object* base,
  int32_t in, int32_t out, double deep)
  : HexagonFrame(base), in_(in), out_(out), deep_(deep)
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;

  radial_points.resize(6, std::vector<Eigen::Vector3d>(3));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (mainData->polygonPoints[i] - mainData->center) / 4;
    radial_points[i][0] = mainData->center + 1 * v;
    radial_points[i][1] = mainData->center + 2 * v;
    radial_points[i][2] = mainData->center + 3 * v;
  }

  middle_points.resize(6, std::vector<Eigen::Vector3d>(2));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (mainData->extraPoints[i][1] - mainData->center) / 4;
    middle_points[i][0] = mainData->center + 1 * v;
    middle_points[i][1] = mainData->center + 2 * v;
  }

  if (in_ == -1 || out_ == -1)
  {
    make_river_begin_end(in_ != -1 ? in_ : out_);
    if (out != -1)
    {
      std::reverse(shore_points.begin(), shore_points.end());
      std::reverse(water_points.begin(), water_points.end());
    }
  }
  else if ((in_ + 3) % 6 == out_)
  {
    make_river_directly(in_, out_);
  }
  else if ((in_ + 2) % 6 == out_ || (out_ + 2) % 6 == in_)
  {
    if ((out_ + 2) % 6 == in_)
      std::swap(in_, out_);

    make_river_angle_2(in_, out_);

    if ((out_ + 2) % 6 == in_)
      std::reverse(shore_points.begin(), shore_points.end());
  }
  else
  {
    if ((out_ + 1) % 6 == in_)
      std::swap(in_, out_);

    make_river_angle_1(in_, out_);

    if ((out_ + 1) % 6 == in_)
      std::reverse(shore_points.begin(), shore_points.end());
  }
}

void Hexoworld::Hexagon::RiversFrame::set_height(int32_t height) {
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 3; ++j)
      base->world.set_new_height_to_point(radial_points[i][j], height, River);
    for (int j = 0; j < 2; ++j)
      base->world.set_new_height_to_point(middle_points[i][j], height, River);
  }
  for (auto& p : shore_points)
    base->world.set_new_height_to_point(p, height, River);
  for (auto& p : floor_points)
    base->world.set_new_height_to_point(p, height, River);
  for (auto& p : water_points)
    base->world.set_new_height_to_point(p, height, River);
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RiversFrame::get_points() const
{
  std::vector<Eigen::Vector3d> ans;

  for (const Eigen::Vector3d& point : shore_points)
    ans.push_back(point);
  for (const Eigen::Vector3d& point : floor_points)
    ans.push_back(point);
  for (const Eigen::Vector3d& point : water_points)
    ans.push_back(point);

  return ans;
}
std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RiversFrame::get_floor_points() const
{
  return floor_points;
}
std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RiversFrame::get_shore_points() const
{
  return shore_points;
}
std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RiversFrame::get_water_points() const
{
  return water_points;
}

void Hexoworld::Hexagon::RiversFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < shore_points.size() - 2; i++)
  {
    printRect(
      { shore_points[i], shore_points[i + 2] },
      { water_points[i / 2], water_points[i / 2 + 1] },
      TriList, base);
  }
}

void Hexoworld::Hexagon::RiversFrame::make_river_begin_end(uint32_t edge)
{
  omit_point(middle_points[edge][1]);
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[edge][1]);

  Eigen::Vector3d dop = base->world.dop_height(River);

  shore_points.push_back(middle_points[edge][0] + dop);
  shore_points.push_back(middle_points[edge][0] + dop);
  water_points.push_back(middle_points[edge][0] + dop);

  shore_points.push_back(radial_points[(edge + 1) % 6][1] + dop);
  shore_points.push_back(radial_points[edge][1] + dop);
  water_points.push_back(middle_points[edge][1] + dop);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[edge][2] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[edge][0] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[edge][1] + dop);
}
void Hexoworld::Hexagon::RiversFrame::make_river_directly(uint32_t in, uint32_t out)
{
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[in][0]);
  omit_point(radial_points[in][0], 0.5);
  omit_point(radial_points[(in + 1) % 6][0], 0.5);
  omit_point(static_cast<Hexagon*>(base)->mainData->center);
  omit_point(radial_points[out][0], 0.5);
  omit_point(radial_points[(out + 1) % 6][0], 0.5);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1]);

  Eigen::Vector3d dop = base->world.dop_height(River);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][0] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][2] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1] + dop);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][2] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][0] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1] + dop);
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_2(uint32_t in, uint32_t out)
{
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1]);
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1]);

  omit_point(middle_points[in][0]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);

  uint32_t middle = (in + 1) % 6;
  omit_point(radial_points[middle][0]);
  omit_point(radial_points[out][0]);

  omit_point(middle_points[middle][0]);

  Eigen::Vector3d dop = base->world.dop_height(River);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][0] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][2] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1] + dop);

  shore_points.push_back(radial_points[in][1] + dop);
  shore_points.push_back(radial_points[(in + 1) % 6][1] + dop);
  water_points.push_back(middle_points[in][1] + dop);

  shore_points.push_back(radial_points[(out + 1) % 6][1] + dop);
  shore_points.push_back(radial_points[out][1] + dop);
  water_points.push_back(middle_points[out][1] + dop);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][2] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][0] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1] + dop);
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_1(uint32_t in, uint32_t out)
{
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1]);
  omit_point(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1]);

  omit_point(middle_points[in][1], 0.75);
  omit_point(middle_points[out][1], 0.75);

  omit_point(middle_points[in][0], 0.75);
  omit_point(middle_points[out][0], 0.75);

  omit_point(radial_points[out][1], 0.75);
  omit_point(radial_points[out][0], 0.75);

  Eigen::Vector3d dop = base->world.dop_height(River);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][0] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][2] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[in][1] + dop);

  shore_points.push_back(radial_points[out][0] + dop);
  shore_points.push_back(radial_points[out][2] + dop);
  water_points.push_back(radial_points[out][1] + dop);

  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][2] + dop);
  shore_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][0] + dop);
  water_points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[out][1] + dop);
}

void Hexoworld::Hexagon::RiversFrame::omit_point(Eigen::Vector3d& point, double deep)
{
  /*Eigen::Vector3d old_point = point;
  point += deep * deep_ * (-base->world.heightDirection_);
  if (Points::get_instance().in_points(old_point))
  {
    Points::get_instance().update_point(
      Points::get_instance().get_id_point(old_point, base),
      point);
  }

  floor_points.push_back(point);*/
}

//struct FloodFrame----------------------------------------

Hexoworld::Hexagon::FloodFrame::FloodFrame(Object* base, double water_level) 
  : HexagonFrame(base), water_level(water_level)
{
  for (int i = 0; i < 6; ++i)
  {
    waterPoints.push_back(static_cast<Hexagon*>(base)->mainData->polygonPoints[i] + 
      base->world.heightDirection_ * water_level);
    for (int j = 0; j < static_cast<Hexagon*>(base)->mainData->extraPoints[i].size(); ++j)
      waterPoints.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[i][j] + 
        base->world.heightDirection_ * water_level);
  }
  waterPoints.push_back(static_cast<Hexagon*>(base)->mainData->center + 
    base->world.heightDirection_ * water_level);

  for (int i = 0; i < waterPoints.size(); ++i)
    Points::get_instance().get_id_point(waterPoints[i], base);
}

void Hexoworld::Hexagon::FloodFrame::set_height(int32_t height)
{
  for (auto& p : waterPoints)
  {
    p -= base->world.heightDirection_ * water_level;
    base->world.set_new_height_to_point(p, height, Flood);
    p += base->world.heightDirection_ * water_level;
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::FloodFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& p : waterPoints)
    answer.push_back(p);

  return answer;
}

void Hexoworld::Hexagon::FloodFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < waterPoints.size() - 1; ++i)
    printTri(waterPoints.back(),
      waterPoints[i],
      waterPoints[(i + 1) % (waterPoints.size() - 1)],
      TriList, 
      base);

  std::vector<Eigen::Vector3d> points;
  for (int i = 0; i < 6; ++i)
  {
    points.push_back(static_cast<Hexagon*>(base)->mainData->polygonPoints[i]);
    for (int j = 0; j < static_cast<Hexagon*>(base)->mainData->extraPoints[i].size(); ++j)
      points.push_back(static_cast<Hexagon*>(base)->mainData->extraPoints[i][j]);
  }

  for (int i = 0; i < waterPoints.size() - 1; ++i)
    printRect(
      {points[i], points[(i + 1) % points.size()]},
      {waterPoints[i], waterPoints[(i + 1) % points.size()]},
      TriList, base);
}

//struct RoadFrame-----------------------------------------

Hexoworld::Hexagon::RoadFrame::RoadFrame(Object* object, std::vector<uint32_t> edges)
  : HexagonFrame(object)
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;
  radial_points.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    radial_points[i] = mainData->center + (mainData->polygonPoints[i] - mainData->center) / 2;
  }
  middle_points.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    middle_points[i] = mainData->center + (mainData->extraPoints[i][1] - mainData->center) / 3;
  }

  crossroads.resize(6);
  for (uint32_t i = 0; i < 6; ++i)
    crossroads[i] = middle_points[i] + base->world.dop_height(Road);
  inputs.resize(6, { Eigen::Vector3d(0, 0, 0), Eigen::Vector3d(0, 0, 0) });
  endOfFence.resize(6, { Eigen::Vector3d(0, 0, 0), Eigen::Vector3d(0, 0, 0) });
  middleFence.resize(6, { Eigen::Vector3d(0, 0, 0), Eigen::Vector3d(0, 0, 0) });

  isRoad.resize(6, false);
  for (uint32_t i : edges)
    add_road(i);
}

void Hexoworld::Hexagon::RoadFrame::add_road(uint32_t ind)
{
  if (!isRoad[ind])
  {
    isRoad[ind] = true;
    init_road(ind);
    if (isRoad[(ind + 1) % 6])
      endOfFence[(ind + 1) % 6].first = radial_points[(ind + 1) % 6] + base->world.dop_height(Road);
    if (isRoad[(ind + 5) % 6])
      endOfFence[(ind + 5) % 6].second = radial_points[ind] + base->world.dop_height(Road);
  }
}

void Hexoworld::Hexagon::RoadFrame::init_road(uint32_t ind)
{
  Eigen::Vector3d dop = base->world.dop_height(Road);
  inputs[ind] = {
    static_cast<Hexagon*>(base)->mainData->extraPoints[ind][0] + dop,
    static_cast<Hexagon*>(base)->mainData->extraPoints[ind][2] + dop
  };

  if (isRoad[(ind + 5) % 6])
    endOfFence[ind].first = radial_points[ind] + dop;
  else
    endOfFence[ind].first = middle_points[(ind + 5) % 6] + dop;

  if (isRoad[(ind + 1) % 6])
    endOfFence[ind].second = radial_points[(ind + 1) % 6] + dop;
  else
    endOfFence[ind].second = middle_points[(ind + 1) % 6] + dop;

  middleFence[ind] = {
    inputs[ind].first + (inputs[ind].second - inputs[ind].first) / 2,
    crossroads[ind]
  };
}

void Hexoworld::Hexagon::RoadFrame::set_height(int32_t height)
{
  for (auto& p : crossroads)
    base->world.set_new_height_to_point(p, height, Road);

  for (auto& [p1, p2] : inputs)
  {
    base->world.set_new_height_to_point(p1, height, Road);
    base->world.set_new_height_to_point(p2, height, Road);
  }
  for (auto& [p1, p2] : endOfFence)
  {
    base->world.set_new_height_to_point(p1, height, Road);
    base->world.set_new_height_to_point(p2, height, Road);
  }
  for (auto& [p1, p2] : middleFence)
  {
    base->world.set_new_height_to_point(p1, height, Road);
    base->world.set_new_height_to_point(p2, height, Road);
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RoadFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer = {
    static_cast<Hexagon*>(base)->mainData->center + base->world.dop_height(Road)
  };

  for (auto& p : crossroads)
    answer.push_back(p);

  for (auto& [p1, p2] : inputs)
  {
    answer.push_back(p1);
    answer.push_back(p2);
  }
  for (auto& [p1, p2] : endOfFence)
  {
    answer.push_back(p1);
    answer.push_back(p2);
  }
  for (auto& [p1, p2] : middleFence)
  {
    answer.push_back(p1);
    answer.push_back(p2);
  }

  return answer;
}

std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> Hexoworld::Hexagon::RoadFrame::get_inner_fence()
{
  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> answer = middleFence;
  for (int i = 0; i < 6; ++i)
    if (
      isRoad[(i + 0) % 6] &&
      isRoad[(i + 1) % 6] &&
      !isRoad[(i + 2) % 6] &&
      !isRoad[(i + 3) % 6] &&
      !isRoad[(i + 4) % 6] &&
      !isRoad[(i + 5) % 6])
    {
      answer.push_back({ crossroads[i],
        static_cast<Hexagon*>(base)->mainData->center + base->world.dop_height(Road) });
      answer.push_back({ crossroads[(i + 1) % 6],
        static_cast<Hexagon*>(base)->mainData->center + base->world.dop_height(Road) });
    }
  return answer;
}

std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> Hexoworld::Hexagon::RoadFrame::get_outer_fence()
{
  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> answer;

  for (int i = 0; i < 6; ++i) if (isRoad[i])
  {
    answer.push_back({inputs[i].first, endOfFence[i].first});
    answer.push_back({inputs[i].second, endOfFence[i].second});
  }

  for (int i = 0; i < 6; ++i) if (!isRoad[i] && !isRoad[(i + 1) % 6])
  {
    answer.push_back({crossroads[i], crossroads[(i + 1) % 6]});
  }

  return answer;
}

void Hexoworld::Hexagon::RoadFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < 6; ++i)
    printTri(
      static_cast<Hexagon*>(base)->mainData->center + base->world.dop_height(Road),
      crossroads[i], 
      crossroads[(i + 1) % 6],
      TriList, base);

  for (uint32_t ind_road = 0; ind_road < 6; ++ind_road) if (isRoad[ind_road]) {
    printRect(
      inputs[ind_road],
      {
        crossroads[(ind_road + 5) % 6],
        crossroads[(ind_road + 1) % 6]
      },
      TriList, base
    );
  }
}
