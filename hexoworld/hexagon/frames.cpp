#include <hexoworld/hexoworld.hpp>
#include <random>
#include <cmath>

Hexoworld::Hexagon::HexagonFrame::HexagonFrame(Object* base, Eigen::Vector3d center,
  std::shared_ptr<MainData> _mainData)
  : Frame(base), mainData(_mainData)
{
  if (mainData == nullptr)
  {
    mainData = std::make_shared<MainData>();

    std::random_device rd;
    mainData->gen_init = rd();

    mainData->center = center;

    const Eigen::Vector3d& pointDirection = base->world.rowDirection_;
    const Eigen::Vector3d& floatDirection = base->world.colDirection_;
    mainData->outerPoints.resize(6);
    {
      mainData->outerPoints[0] = center + (pointDirection * base->world.size_);
      mainData->outerPoints[1] =
        center + (pointDirection * (0.5 * base->world.size_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->outerPoints[2] =
        center - (pointDirection * (0.5 * base->world.size_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->outerPoints[3] = center - (pointDirection * base->world.size_);
      mainData->outerPoints[4] =
        center - (pointDirection * (0.5 * base->world.size_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
      mainData->outerPoints[5] =
        center + (pointDirection * (0.5 * base->world.size_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.size_));
    }
    mainData->innerPoints.resize(6);
    {
      mainData->innerPoints[0] = center + (pointDirection * base->world.innerSize_);
      mainData->innerPoints[1] =
        center + (pointDirection * (0.5 * base->world.innerSize_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.innerSize_));
      mainData->innerPoints[2] =
        center - (pointDirection * (0.5 * base->world.innerSize_)) +
        (floatDirection * (sqrtf(3) / 2 * base->world.innerSize_));
      mainData->innerPoints[3] = center - (pointDirection * base->world.innerSize_);
      mainData->innerPoints[4] =
        center - (pointDirection * (0.5 * base->world.innerSize_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.innerSize_));
      mainData->innerPoints[5] =
        center + (pointDirection * (0.5 * base->world.innerSize_)) -
        (floatDirection * (sqrtf(3) / 2 * base->world.innerSize_));
    }
    mainData->extraPoints.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      Eigen::Vector3d a = mainData->innerPoints[i];
      Eigen::Vector3d b = mainData->innerPoints[(i + 1) % 6];
      Eigen::Vector3d v = (b - a) / 4;
      mainData->extraPoints[i].resize(3);
      for (int j = 0; j < 3; ++j)
        mainData->extraPoints[i][j] = a + v * (j + 1);
    }

    {
      std::mt19937 gen(mainData->gen_init);
      std::uniform_real_distribution<> dis(-0.2, 0.2);
      double height_move = dis(gen);
      for (Eigen::Vector3d& point : mainData->innerPoints)
      {
        point +=
          base->world.rowDirection_ * dis(gen) +
          base->world.colDirection_ * dis(gen) +
          base->world.heightDirection_ * height_move;
      }
      for (auto& i : mainData->extraPoints)
        for (Eigen::Vector3d& point : i)
          point +=
          base->world.rowDirection_ * dis(gen) +
          base->world.colDirection_ * dis(gen) +
          base->world.heightDirection_ * height_move;
      mainData->center +=
        base->world.rowDirection_ * dis(gen) +
        base->world.colDirection_ * dis(gen) +
        base->world.heightDirection_ * height_move;
    }

    mainData->innerPointsId.resize(6);
    for (int i = 0; i < 6; ++i)
      mainData->innerPointsId[i] =
      Points::get_instance().get_id_point(
        mainData->innerPoints[i], base);

    mainData->outerPointsId.resize(6);
    for (int i = 0; i < 6; ++i)
      mainData->outerPointsId[i] =
      Points::get_instance().get_id_point(
        mainData->outerPoints[i], base);

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

void Hexoworld::Hexagon::HexagonFrame::set_height(int32_t height)
{
  for (int i = 0; i < 6; ++i)
  {
    mainData->innerPoints[i] = Points::get_instance().get_point(mainData->innerPointsId[i]);
    set_new_height_to_point(mainData->innerPoints[i], height);
    Points::get_instance().update_point(mainData->innerPointsId[i], mainData->innerPoints[i]);

    for (int j = 0; j < 3; ++j)
    {
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);
      set_new_height_to_point(mainData->extraPoints[i][j], height);
      Points::get_instance().update_point(mainData->extraPointsId[i][j], mainData->extraPoints[i][j]);
    }
  }
  set_new_height_to_point(mainData->center, height);
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::HexagonFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (int i = 0; i < 6; ++i)
  {
    mainData->innerPoints[i] = Points::get_instance().get_point(mainData->innerPointsId[i]);
    answer.push_back(mainData->innerPoints[i]);

    for (int j = 0; j < 3; ++j)
    {
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);
      answer.push_back(mainData->extraPoints[i][j]);
    }
  }
  answer.push_back(mainData->center);
  return answer;
}

uint32_t Hexoworld::Hexagon::HexagonFrame::get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2)
{
  if (vertex1 > vertex2)
    std::swap(vertex1, vertex2);

  if (vertex1 == 0 && vertex2 == 5)
    return 5;
  else
    return vertex1;
}
void Hexoworld::Hexagon::HexagonFrame::set_new_height_to_point(Eigen::Vector3d& point, int32_t height)
{
  int32_t last_height =
    round(base->world.heightDirection_
      .dot(point - base->world.origin_) /
      base->world.heightStep_);

  point += (height - last_height) *
    base->world.heightStep_ *
    base->world.heightDirection_;
}

//struct UsualType-----------------------------------------

Hexoworld::Hexagon::UsualFrame::UsualFrame(Object* base, Eigen::Vector3d center,
  std::shared_ptr<MainData> mainData)
  : HexagonFrame(base, center, mainData)
{}

void Hexoworld::Hexagon::UsualFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < 6; ++i)
    mainData->innerPoints[i] = Points::get_instance().get_point(mainData->innerPointsId[i]);
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < mainData->extraPoints[i].size(); ++j)
      mainData->extraPoints[i][j] = Points::get_instance().get_point(mainData->extraPointsId[i][j]);

  std::vector<Eigen::Vector3d> points;
  for (int i = 0; i < 6; ++i)
  {
    points.push_back(mainData->innerPoints[i]);
    for (int j = 0; j < mainData->extraPoints[i].size(); ++j)
      points.push_back(mainData->extraPoints[i][j]);
  }

  for (int i = 0; i < points.size(); ++i)
    printTri(mainData->center, points[i], points[(i + 1) % points.size()], TriList, base);
}

//struct RiversType----------------------------------------

Hexoworld::Hexagon::RiversFrame::RiversFrame(Object* base, Eigen::Vector3d center,
  int32_t in, int32_t out, std::shared_ptr<MainData> mainData)
  : HexagonFrame(base, center, mainData), in_(in), out_(out), deep_(0.2)
{
  radial_points.resize(6, std::vector<Eigen::Vector3d>(3));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (mainData->innerPoints[i] - mainData->center) / 4;
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
      std::reverse(shore_points.begin(), shore_points.end());
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

  for (int i = 0; i < shore_points.size(); i += 2)
    water_points.push_back(shore_points[i] +
      (shore_points[i + 1] - shore_points[i]) / 2);
}

void Hexoworld::Hexagon::RiversFrame::set_height(int32_t height) {
  HexagonFrame::set_height(height);

  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 3; ++j)
      set_new_height_to_point(radial_points[i][j], height);
    for (int j = 0; j < 2; ++j)
      set_new_height_to_point(middle_points[i][j], height);
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RiversFrame::get_points() const
{
  std::vector<Eigen::Vector3d> ans;
  for (const Eigen::Vector3d& point : HexagonFrame::get_points())
    ans.push_back(point);

  for (const auto& direction : radial_points)
    for (const Eigen::Vector3d& point : direction)
      ans.push_back(point);
  for (const auto& direction : middle_points)
    for (const Eigen::Vector3d& point : direction)
      ans.push_back(point);
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
  for (uint32_t ind1 = 0; ind1 < 6; ++ind1)
  {
    uint32_t ind2 = (ind1 + 1) % 6;

    printTri(
      mainData->innerPoints[ind1],
      radial_points[ind1][2],
      mainData->extraPoints[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind1][1],
      radial_points[ind1][2],
      mainData->extraPoints[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind1][1],
      mainData->extraPoints[ind1][1],
      mainData->extraPoints[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind1][1],
      mainData->extraPoints[ind1][1],
      middle_points[ind1][1],
      TriList, base);
    printTri(
      radial_points[ind2][1],
      mainData->extraPoints[ind1][1],
      middle_points[ind1][1],
      TriList, base);
    printTri(
      radial_points[ind2][1],
      mainData->extraPoints[ind1][1],
      mainData->extraPoints[ind1][2],
      TriList, base);
    printTri(
      radial_points[ind2][1],
      radial_points[ind2][2],
      mainData->extraPoints[ind1][2],
      TriList, base);
    printTri(
      mainData->innerPoints[ind2],
      radial_points[ind2][2],
      mainData->extraPoints[ind1][2],
      TriList, base);
    printTri(
      radial_points[ind1][1],
      middle_points[ind1][1],
      middle_points[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind2][1],
      middle_points[ind1][1],
      middle_points[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind1][0],
      radial_points[ind1][1],
      middle_points[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind2][0],
      radial_points[ind2][1],
      middle_points[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind1][0],
      mainData->center,
      middle_points[ind1][0],
      TriList, base);
    printTri(
      radial_points[ind2][0],
      mainData->center,
      middle_points[ind1][0],
      TriList, base);
  }
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
  omit_point(mainData->extraPoints[edge][1]);

  shore_points.push_back(middle_points[edge][0]);
  shore_points.push_back(middle_points[edge][0]);
  shore_points.push_back(radial_points[(edge + 1) % 6][1]);
  shore_points.push_back(radial_points[edge][1]);
  shore_points.push_back(mainData->extraPoints[edge][2]);
  shore_points.push_back(mainData->extraPoints[edge][0]);
}
void Hexoworld::Hexagon::RiversFrame::make_river_directly(uint32_t in, uint32_t out)
{
  omit_point(mainData->extraPoints[in][1]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[in][0]);
  omit_point(radial_points[in][0], 0.5);
  omit_point(radial_points[(in + 1) % 6][0], 0.5);
  omit_point(mainData->center);
  omit_point(radial_points[out][0], 0.5);
  omit_point(radial_points[(out + 1) % 6][0], 0.5);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);
  omit_point(mainData->extraPoints[out][1]);

  shore_points.push_back(mainData->extraPoints[in][0]);
  shore_points.push_back(mainData->extraPoints[in][2]);
  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  shore_points.push_back(middle_points[(in + 5) % 6][0]);
  shore_points.push_back(middle_points[(in + 1) % 6][0]);

  shore_points.push_back(radial_points[(in + 5) % 6][0]);
  shore_points.push_back(radial_points[(out + 5) % 6][0]);

  shore_points.push_back(middle_points[(out + 1) % 6][0]);
  shore_points.push_back(middle_points[(out + 5) % 6][0]);
  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(radial_points[out][1]);
  shore_points.push_back(mainData->extraPoints[out][2]);
  shore_points.push_back(mainData->extraPoints[out][0]);
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_2(uint32_t in, uint32_t out)
{
  omit_point(mainData->extraPoints[in][1]);
  omit_point(mainData->extraPoints[out][1]);

  omit_point(middle_points[in][0]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);

  uint32_t middle = (in + 1) % 6;
  omit_point(radial_points[middle][0]);
  omit_point(radial_points[out][0]);

  omit_point(middle_points[middle][0]);

  shore_points.push_back(mainData->extraPoints[in][0]);
  shore_points.push_back(mainData->extraPoints[in][2]);
  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  shore_points.push_back(radial_points[in][0]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  shore_points.push_back(mainData->center);
  shore_points.push_back(middle_points[middle][1]);
  shore_points.push_back(radial_points[(out + 1) % 6][0]);
  shore_points.push_back(radial_points[out][1]);
  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(radial_points[out][1]);
  shore_points.push_back(mainData->extraPoints[out][2]);
  shore_points.push_back(mainData->extraPoints[out][0]);
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_1(uint32_t in, uint32_t out)
{
  omit_point(mainData->extraPoints[in][1]);
  omit_point(mainData->extraPoints[out][1]);

  omit_point(middle_points[in][1], 0.75);
  omit_point(middle_points[out][1], 0.75);

  omit_point(middle_points[in][0], 0.75);
  omit_point(middle_points[out][0], 0.75);

  omit_point(radial_points[out][1], 0.75);
  omit_point(radial_points[out][0], 0.75);

  shore_points.push_back(mainData->extraPoints[in][0]);
  shore_points.push_back(mainData->extraPoints[in][2]);
  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(mainData->extraPoints[in][2]);
  shore_points.push_back(radial_points[in][0]);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(mainData->center);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(radial_points[(out + 1) % 6][0]);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(mainData->extraPoints[out][0]);
  shore_points.push_back(mainData->extraPoints[out][2]);
  shore_points.push_back(mainData->extraPoints[out][0]);
}

void Hexoworld::Hexagon::RiversFrame::omit_point(Eigen::Vector3d& point, double deep)
{
  Eigen::Vector3d old_point = point;
  point += deep * deep_ * (-base->world.heightDirection_);
  if (Points::get_instance().in_points(old_point))
  {
    Points::get_instance().update_point(
      Points::get_instance().get_id_point(old_point, base),
      point);
  }

  floor_points.push_back(point);
}