#include <hexoworld/hexoworld.hpp>
#include <hexoworld/hexagon_grid.hpp>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <cmath>

uint32_t Hexoworld::HexagonGrid::connect(Eigen::Vector3d point, std::shared_ptr<HexagonGrid::Object> object)
{
  return HexagonGrid::Points::get_instance().connect_point_with_object(point, object);
}
void Hexoworld::HexagonGrid::printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
  std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
  std::vector<uint16_t>& TriList)
{
  uint32_t id1;
  id1 = Points::get_instance().get_id_point(a.first);
  
  uint32_t id2;
  id2 = Points::get_instance().get_id_point(a.second);
  
  uint32_t id3;
  id3 = Points::get_instance().get_id_point(b.first);
  
  uint32_t id4;
  id4 = Points::get_instance().get_id_point(b.second);

  TriList.push_back(id1);
  TriList.push_back(id2);
  TriList.push_back(id3);

  TriList.push_back(id3);
  TriList.push_back(id2);
  TriList.push_back(id1);

  TriList.push_back(id2);
  TriList.push_back(id3);
  TriList.push_back(id4);

  TriList.push_back(id4);
  TriList.push_back(id3);
  TriList.push_back(id2);
}
void Hexoworld::HexagonGrid::printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  std::vector<uint16_t>& TriList)
{
  uint32_t id1;
  id1 = Points::get_instance().get_id_point(a);
  
  uint32_t id2;
  id2 = Points::get_instance().get_id_point(b);
  
  uint32_t id3;
  id3 = Points::get_instance().get_id_point(c);

  TriList.push_back(id1);
  TriList.push_back(id2);
  TriList.push_back(id3);

  TriList.push_back(id3);
  TriList.push_back(id2);
  TriList.push_back(id1);
}

//class Points---------------------------------------------
uint32_t Hexoworld::HexagonGrid::Points::connect_point_with_object(Eigen::Vector3d p,
  std::shared_ptr<Object> object)
{
  uint32_t id;
  if (point_to_id.find(p) == point_to_id.end())
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_objects.push_back(std::vector<std::shared_ptr<Object>>());
    id_to_objects.back().push_back(object);
  }
  else
  {
    id = point_to_id[p];
    id_to_objects[id].push_back(object);
  }
  return id;
}

bool Hexoworld::HexagonGrid::Points::in_points(Eigen::Vector3d p) const
{
  return (point_to_id.find(p) != point_to_id.end());
}

uint32_t Hexoworld::HexagonGrid::Points::get_id_point(Eigen::Vector3d p)
{
  uint32_t id;
  if (point_to_id.find(p) == point_to_id.end())
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_objects.push_back(std::vector<std::shared_ptr<Object>>());
  }
  else
  {
    id = point_to_id[p];
  }
  return id;
}

std::vector<std::shared_ptr<Hexoworld::HexagonGrid::Object>> 
Hexoworld::HexagonGrid::Points::get_objects(uint32_t id)
{
  if (id >= id_to_objects.size())
    throw std::invalid_argument("wrong id");
  return id_to_objects[id];
}

Eigen::Vector3d 
Hexoworld::HexagonGrid::Points::get_point(uint32_t id) const
{
  if (id >= id_to_point.size())
    throw std::invalid_argument("wrong id");
  return id_to_point[id];
}

void Hexoworld::HexagonGrid::Points::update_point(uint32_t id, Eigen::Vector3d new_point)
{
  Eigen::Vector3d point = Points::get_instance().id_to_point[id];
  Points::get_instance().point_to_id.erase(point);
  Points::get_instance().point_to_id[new_point] = id;
  Points::get_instance().id_to_point[id] = new_point;
}

//struct UsualType-----------------------------------------

Hexoworld::HexagonGrid::UsualType::UsualType(Hexagon& hexagon)
: HexagonType(hexagon){}
void Hexoworld::HexagonGrid::UsualType::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (int i = 0; i < 6; ++i)
    base.innerPoints[i] = Points::get_instance().get_point(base.innerPointsId[i]);
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < base.extraPoints[i].size(); ++j)
      base.extraPoints[i][j] = Points::get_instance().get_point(base.extraPointsId[i][j]);

  std::vector<Eigen::Vector3d> points;
  for (int i = 0; i < 6; ++i)
  {
    points.push_back(base.innerPoints[i]);
    for (int j = 0; j < base.extraPoints[i].size(); ++j)
      points.push_back(base.extraPoints[i][j]);
  }

  for (int i = 0; i < points.size(); ++i)
    printTri(base.center, points[i], points[(i + 1) % points.size()],
      TriList);
}

//struct RiversType----------------------------------------

Hexoworld::HexagonGrid::RiversType::RiversType(Hexagon& hexagon, int32_t in, int32_t out)
: HexagonType(hexagon), in_(in), out_(out), deep_(0.2)
{
  radial_points.resize(6, std::vector<Eigen::Vector3d>(3));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (base.innerPoints[i] - base.center) / 4;
    radial_points[i][0] = base.center + 1 * v;
    radial_points[i][1] = base.center + 2 * v;
    radial_points[i][2] = base.center + 3 * v;
  }

  middle_points.resize(6, std::vector<Eigen::Vector3d>(2));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (base.extraPoints[i][1] - base.center) / 4;
    middle_points[i][0] = base.center + 1 * v;
    middle_points[i][1] = base.center + 2 * v;
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
}

void Hexoworld::HexagonGrid::RiversType::set_height(int32_t height) {
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 3; ++j)
      base.set_new_height_to_point(radial_points[i][j], height);
    for (int j = 0; j < 2; ++j)
      base.set_new_height_to_point(middle_points[i][j], height);
  }
}

std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::RiversType::get_points() const
{
  return shore_points;
}

void Hexoworld::HexagonGrid::RiversType::print_in_triList(std::vector<uint16_t>& TriList) const
{
  for (uint32_t ind1 = 0; ind1 < 6; ++ind1)
  {
    uint32_t ind2 = (ind1 + 1) % 6;

    printTri(
      base.innerPoints[ind1], 
      radial_points[ind1][2],
      base.extraPoints[ind1][0],
      TriList);
    printTri(
      radial_points[ind1][1],
      radial_points[ind1][2],
      base.extraPoints[ind1][0],
      TriList);
    printTri(
      radial_points[ind1][1],
      base.extraPoints[ind1][1],
      base.extraPoints[ind1][0],
      TriList);
    printTri(
      radial_points[ind1][1],
      base.extraPoints[ind1][1],
      middle_points[ind1][1],
      TriList);
    printTri(
      radial_points[ind2][1],
      base.extraPoints[ind1][1],
      middle_points[ind1][1],
      TriList);
    printTri(
      radial_points[ind2][1],
      base.extraPoints[ind1][1],
      base.extraPoints[ind1][2],
      TriList);
    printTri(
      radial_points[ind2][1],
      radial_points[ind2][2],
      base.extraPoints[ind1][2],
      TriList);
    printTri(
      base.innerPoints[ind2],
      radial_points[ind2][2],
      base.extraPoints[ind1][2],
      TriList);
    printTri(
      radial_points[ind1][1],
      middle_points[ind1][1],
      middle_points[ind1][0],
      TriList);
    printTri(
      radial_points[ind2][1],
      middle_points[ind1][1],
      middle_points[ind1][0],
      TriList);
    printTri(
      radial_points[ind1][0],
      radial_points[ind1][1],
      middle_points[ind1][0],
      TriList);
    printTri(
      radial_points[ind2][0],
      radial_points[ind2][1],
      middle_points[ind1][0],
      TriList);
    printTri(
      radial_points[ind1][0],
      base.center,
      middle_points[ind1][0],
      TriList);
    printTri(
      radial_points[ind2][0],
      base.center,
      middle_points[ind1][0],
      TriList);
  }
}

void Hexoworld::HexagonGrid::RiversType::make_river_begin_end(uint32_t edge)
{
  omit_point(middle_points[edge][1]);
  omit_point(base.extraPoints[edge][1]);

  shore_points.push_back(middle_points[edge][0]);
  shore_points.push_back(middle_points[edge][0]);
  shore_points.push_back(radial_points[(edge + 1) % 6][1]);
  shore_points.push_back(radial_points[edge][1]);
  shore_points.push_back(base.extraPoints[edge][2]);
  shore_points.push_back(base.extraPoints[edge][0]);
}
void Hexoworld::HexagonGrid::RiversType::make_river_directly(uint32_t in, uint32_t out)
{
  omit_point(base.extraPoints[in][1]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[in][0]);
  omit_point(radial_points[in][0], 0.5);
  omit_point(radial_points[(in + 1) % 6][0], 0.5);
  omit_point(base.center);
  omit_point(radial_points[out][0], 0.5);
  omit_point(radial_points[(out + 1) % 6][0], 0.5);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);
  omit_point(base.extraPoints[out][1]);

  shore_points.push_back(base.extraPoints[in][0]);
  shore_points.push_back(base.extraPoints[in][2]);
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
  shore_points.push_back(base.extraPoints[out][2]);
  shore_points.push_back(base.extraPoints[out][0]);
}
void Hexoworld::HexagonGrid::RiversType::make_river_angle_2(uint32_t in, uint32_t out)
{
  omit_point(base.extraPoints[in][1]);
  omit_point(base.extraPoints[out][1]);

  omit_point(middle_points[in][0]);
  omit_point(middle_points[in][1]);
  omit_point(middle_points[out][0]);
  omit_point(middle_points[out][1]);

  uint32_t middle = (in + 1) % 6;
  omit_point(radial_points[middle][0]);
  omit_point(radial_points[out][0]);

  omit_point(middle_points[middle][0]);

  shore_points.push_back(base.extraPoints[in][0]);
  shore_points.push_back(base.extraPoints[in][2]);
  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  shore_points.push_back(radial_points[in][0]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  shore_points.push_back(base.center);
  shore_points.push_back(middle_points[middle][1]);
  shore_points.push_back(radial_points[(out + 1) % 6][0]);
  shore_points.push_back(radial_points[out][1]);
  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(radial_points[out][1]);
  shore_points.push_back(base.extraPoints[out][2]);
  shore_points.push_back(base.extraPoints[out][0]);
}
void Hexoworld::HexagonGrid::RiversType::make_river_angle_1(uint32_t in, uint32_t out)
{
  omit_point(base.extraPoints[in][1]);
  omit_point(base.extraPoints[out][1]);
  
  omit_point(middle_points[in][1], 0.75);
  omit_point(middle_points[out][1], 0.75);

  omit_point(middle_points[in][0], 0.75);
  omit_point(middle_points[out][0], 0.75);
  
  omit_point(radial_points[out][1], 0.75);
  omit_point(radial_points[out][0], 0.75);

  shore_points.push_back(base.extraPoints[in][0]);
  shore_points.push_back(base.extraPoints[in][2]);
  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(base.extraPoints[in][2]);
  shore_points.push_back(radial_points[in][0]);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(base.center);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(radial_points[(out + 1) % 6][0]);
  shore_points.push_back(radial_points[out][2]);
  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(base.extraPoints[out][0]);
  shore_points.push_back(base.extraPoints[out][2]);
  shore_points.push_back(base.extraPoints[out][0]);
}

void Hexoworld::HexagonGrid::RiversType::omit_point(Eigen::Vector3d& point, double deep)
{
  Eigen::Vector3d old_point = point;
  point += deep * deep_ * (-base.world.hexagonGrid_->heightDirection_);
  if (Points::get_instance().in_points(old_point))
  {
    Points::get_instance().update_point(
      Points::get_instance().get_id_point(old_point),
      point);
  }

  floor_points.push_back(point);
}
//struct Hexagon-------------------------------------------

Hexoworld::HexagonGrid::Hexagon::Hexagon(Hexoworld& hexoworld,
  float big_size, float small_size,
  Eigen::Vector3d center,
  Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection,
  Coord coord)
  : center(center), world(hexoworld), coord(coord) {
  if (abs(pointDirection.dot(floatDirection)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("pointTop and floatTop not perpendicular");

  pointDirection.normalize();
  floatDirection.normalize();
  outerPoints.resize(6);
  {
    outerPoints[0] = center + (pointDirection * big_size);
    outerPoints[1] =
      center + (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[2] =
      center - (pointDirection * (0.5 * big_size)) +
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[3] = center - (pointDirection * big_size);
    outerPoints[4] =
      center - (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size));
    outerPoints[5] =
      center + (pointDirection * (0.5 * big_size)) -
      (floatDirection * (sqrtf(3) / 2 * big_size));
  }
  innerPoints.resize(6);
  {
    innerPoints[0] = center + (pointDirection * small_size);
    innerPoints[1] =
      center + (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[2] =
      center - (pointDirection * (0.5 * small_size)) +
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[3] = center - (pointDirection * small_size);
    innerPoints[4] =
      center - (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size));
    innerPoints[5] =
      center + (pointDirection * (0.5 * small_size)) -
      (floatDirection * (sqrtf(3) / 2 * small_size));
  }
  extraPoints.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d a = innerPoints[i];
    Eigen::Vector3d b = innerPoints[(i + 1) % 6];
    Eigen::Vector3d v = (b - a) / 4;
    extraPoints[i].resize(3);
    for (int j = 0; j < 3; ++j)
      extraPoints[i][j] = a + v * (j + 1);
  }

  //disturb_the_points();
  hexagonType = std::make_shared<UsualType>(*this);
}
void Hexoworld::HexagonGrid::Hexagon::disturb_the_points()
{
  if (!was_init_gen)
  {
    was_init_gen = true;
    std::random_device rd;
    gen_init = rd();
  }
  std::mt19937 gen(gen_init);
  std::uniform_real_distribution<> dis(-0.1, 0.1);
  double height_move = dis(gen);
  for (Eigen::Vector3d& point : innerPoints)
  {
    point +=
      world.hexagonGrid_->rowDirection_ * dis(gen) +
      world.hexagonGrid_->colDirection_ * dis(gen) +
      world.hexagonGrid_->heightDirection_ * height_move;
  }
  for (auto& i : extraPoints)
    for (Eigen::Vector3d& point : i)
      point +=
      world.hexagonGrid_->rowDirection_ * dis(gen) +
      world.hexagonGrid_->colDirection_ * dis(gen) +
      world.hexagonGrid_->heightDirection_ * height_move;
  center +=
    world.hexagonGrid_->rowDirection_ * dis(gen) +
    world.hexagonGrid_->colDirection_ * dis(gen) +
    world.hexagonGrid_->heightDirection_ * height_move;
}
void Hexoworld::HexagonGrid::Hexagon::connect_points(std::shared_ptr<Hexagon> ptr)
{
  innerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    innerPointsId[i] = connect(innerPoints[i], ptr);
  outerPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    outerPointsId[i] = connect(outerPoints[i], ptr);
  extraPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    extraPointsId[i].resize(extraPoints[i].size());
    for (int j = 0; j < extraPoints[i].size(); ++j)
      extraPointsId[i][j] = connect(extraPoints[i][j], ptr);
  }
}
void Hexoworld::HexagonGrid::Hexagon::set_height(int32_t height) {
  set_new_height_to_point(center, height);

  for (int i = 0; i < 6; ++i)
  {
    uint32_t id = innerPointsId[i];

    Eigen::Vector3d p = Points::get_instance().get_point(id);
    set_new_height_to_point(p, height);
    Points::get_instance().update_point(id, p);
    innerPoints[i] = p;
  }

  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < extraPointsId[i].size(); ++j)
    {
      uint32_t id = extraPointsId[i][j];

      Eigen::Vector3d p = Points::get_instance().get_point(id);
      set_new_height_to_point(p, height);
      Points::get_instance().update_point(id, p);
      extraPoints[i][j] = p;
    }

  hexagonType->set_height(height);
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::Hexagon::make_river(int32_t in, int32_t out)
{
  hexagonType.reset();
  hexagonType = std::make_shared<RiversType>(*this, in, out);
  return std::static_pointer_cast<RiversType>(hexagonType)->
    get_points();
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::Hexagon::get_points()
{
  std::vector<Eigen::Vector3d> answer;
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d p = Points::get_instance().get_point(innerPointsId[i]);
    answer.push_back(p);
  }
  answer.push_back(center);
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < extraPointsId[i].size(); ++j)
    {
      Eigen::Vector3d p = Points::get_instance().get_point(extraPointsId[i][j]);
      answer.push_back(p);
    }
  return answer;
}
uint32_t Hexoworld::HexagonGrid::Hexagon::get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2)
{
  if (vertex1 > vertex2)
    std::swap(vertex1, vertex2);

  if (vertex1 == 0 && vertex2 == 5)
    return 5;
  else
    return vertex1;
}
void Hexoworld::HexagonGrid::Hexagon::print_in_triList(std::vector<uint16_t>& TriList) const
{
  hexagonType->print_in_triList(TriList);
}
void Hexoworld::HexagonGrid::Hexagon::set_new_height_to_point(Eigen::Vector3d& point, int32_t height)
{
  int32_t last_height =
    round(world.hexagonGrid_->heightDirection_
      .dot(point - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);

  point += (height - last_height) *
    world.hexagonGrid_->heightStep_ *
    world.hexagonGrid_->heightDirection_;
}
//struct Triangle------------------------------------------

Hexoworld::HexagonGrid::Triangle::Triangle(Hexoworld& hexoworld, 
  Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c)
: world(hexoworld) {
  std::vector<uint32_t> ids = {
    Points::get_instance().get_id_point(a) ,
    Points::get_instance().get_id_point(b) ,
    Points::get_instance().get_id_point(c) };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

Hexoworld::HexagonGrid::Triangle::Triangle(Hexoworld& hexoworld,
  uint32_t aId, uint32_t bId, uint32_t cId)
  : world(hexoworld) {
  std::vector<uint32_t> ids = { aId , bId , cId };
  std::sort(ids.begin(), ids.end());
  AId = ids[0];
  BId = ids[1];
  CId = ids[2];
}

bool Hexoworld::HexagonGrid::Triangle::operator<(const Triangle& rhs) const
{
  if (AId < rhs.AId)
    return true;
  if (AId > rhs.AId)
    return false;

  if (BId < rhs.BId)
    return true;
  if (BId > rhs.BId)
    return false;

  if (CId < rhs.CId)
    return true;
  return false;
}

void Hexoworld::HexagonGrid::Triangle::print_in_triList(
  std::vector<uint16_t>& TriList) const {
  Eigen::Vector3d a = Points::get_instance().get_point(AId);
  Eigen::Vector3d b = Points::get_instance().get_point(BId);
  Eigen::Vector3d c = Points::get_instance().get_point(CId);

  int32_t heightA =
    round(world.hexagonGrid_->heightDirection_.dot(
      a - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);
  int32_t heightB =
    round(world.hexagonGrid_->heightDirection_.dot(
      b - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);
  int32_t heightC =
    round(world.hexagonGrid_->heightDirection_.dot(
      c - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);

  std::vector<std::pair<int32_t, Eigen::Vector3d>> points;
  points.push_back({ heightA, a });
  points.push_back({ heightB, b });
  points.push_back({ heightC, c });
  std::sort(points.begin(), points.end(), 
    [](const std::pair<int32_t, Eigen::Vector3d>& a,
      const std::pair<int32_t, Eigen::Vector3d>& b)
    {
      if (a.first != b.first)
        return a.first < b.first;
      else
      {
        EigenVector3dComp cmp;
        return cmp(a.second, b.second);
      }
    }
  );

  if (points[0].first == points[2].first)
  {
    printTri(
      points[0].second, 
      points[1].second,
      points[2].second,
      TriList);
  }
  else
  {
    if (points[0].first != points[1].first &&
      points[1].first != points[2].first)
    {
      uint32_t nTerraces =
        (points[2].first - points[0].first) *
        (world.hexagonGrid_->nTerracesOnHeightStep_ + 1) - 1;
      Eigen::Vector3d step =
        (points[2].second -
          points[0].second) / (nTerraces + 1);
      Eigen::Vector3d platformVector =
        world.hexagonGrid_->heightDirection_
        .cross(step)
        .cross(world.hexagonGrid_->heightDirection_)
        .normalized();
      platformVector *= step.dot(platformVector) / 3;

      Eigen::Vector3d goal = points[0].second +
        step * (world.hexagonGrid_->nTerracesOnHeightStep_ + 1)
        * (points[1].first - points[0].first);

      double len = (points[0].second
        - points[2].second).norm();

      Eigen::Vector3d p1;
      p1 = goal - platformVector;

      Eigen::Vector3d p2;
      p2 = goal + platformVector;


      print_stair(points[1].second, p1, points[0].second,
        points[1].second, goal, points[0].second, TriList, 
        (points[0].first + 1 == points[1].first ? 2 : 3));
      print_stair(points[1].second, p2, points[2].second,
        points[1].second, goal, points[2].second, TriList,
        (points[1].first + 1 == points[2].first ? 2 : 3));
    }
    else
    {
      if (points[0].first == points[1].first)
        print_stair(points[0].second,
          points[1].second,
          points[2].second,
          points[0].second,
          points[1].second,
          points[2].second, 
          TriList, 
          (points[0].first + 1 == points[2].first? 0 : 3));
      else
        print_stair(points[1].second,
          points[2].second,
          points[0].second,
          points[1].second,
          points[2].second,
          points[0].second,
          TriList,
          (points[0].first + 1 == points[2].first ? 0 : 3));
    }
  }
}

void Hexoworld::HexagonGrid::Triangle::print_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
  Eigen::Vector3d a_goal,
  Eigen::Vector3d b_goal,
  Eigen::Vector3d c_goal,
  std::vector<uint16_t>& TriList,
  uint8_t cliff) const
{
  if ((cliff & 1) > 0)
    a = a_goal;
  if ((cliff & 2) > 0)
    b = b_goal;
  Eigen::Vector3d d = c;
  Eigen::Vector3d d_goal = c_goal;
  int32_t heightstart =
    round(world.hexagonGrid_->heightDirection_.dot(
      a_goal - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);
  int32_t heightend =
    round(world.hexagonGrid_->heightDirection_.dot(
      c_goal - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);

  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> stairs;
  stairs.push_back({ a, b });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      abs(heightend - heightstart) *
      (world.hexagonGrid_->nTerracesOnHeightStep_ + 1) - 1;

    Eigen::Vector3d step1, platformVector1;
    if ((cliff & 1) == 0)
    {
      step1 = (c_goal - a_goal) / (nTerraces + 1);
      platformVector1 = world.hexagonGrid_->heightDirection_
        .cross(step1)
        .cross(world.hexagonGrid_->heightDirection_)
        .normalized();
      platformVector1 *= step1.dot(platformVector1) / 3;
    }
    else
    { 
      platformVector1 = step1 = Eigen::Vector3d(0, 0, 0);
    }

    Eigen::Vector3d step2, platformVector2;
    if ((cliff & 2) == 0)
    {
      step2 = (d_goal - b_goal) / (nTerraces + 1);
      platformVector2 = world.hexagonGrid_->heightDirection_
        .cross(step2)
        .cross(world.hexagonGrid_->heightDirection_)
        .normalized();
      platformVector2 *= step2.dot(platformVector2) / 3;
    }
    else
    {
      platformVector2 = step2 = Eigen::Vector3d(0, 0, 0);
    }

    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a - c).norm();
      Eigen::Vector3d p1, p2, p3, p4;

      p1 = a_goal + step1 * i - platformVector1;
      p2 = b_goal + step2 * i - platformVector2;
      p3 = a_goal + step1 * i + platformVector1;
      p4 = b_goal + step2 * i + platformVector2;

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], TriList);
}

//struct Rectangle------------------------------------------

Hexoworld::HexagonGrid::BorderRectangle::BorderRectangle(
  Hexoworld& hexoworld,
  Eigen::Vector3d a, Eigen::Vector3d b,
  Eigen::Vector3d c, Eigen::Vector3d d) 
: world(hexoworld) {
  AId = Points::get_instance().get_id_point(a);
  BId = Points::get_instance().get_id_point(b);
  CId = Points::get_instance().get_id_point(c);
  DId = Points::get_instance().get_id_point(d);
}

Hexoworld::HexagonGrid::BorderRectangle::BorderRectangle(
  Hexoworld& hexoworld,
  uint32_t aId, uint32_t bId,
  uint32_t cId, uint32_t dId)
  : world(hexoworld) {
  AId = aId;
  BId = bId;
  CId = cId;
  DId = dId;
}

bool Hexoworld::HexagonGrid::BorderRectangle::operator<(const BorderRectangle& rhs) const
{
  std::vector<uint32_t> ids_lhs = { AId , BId , CId, DId };
  std::vector<uint32_t> ids_rhs = { rhs.AId , rhs.BId ,
    rhs.CId, rhs.DId };

  std::sort(ids_lhs.begin(), ids_lhs.end());
  std::sort(ids_rhs.begin(), ids_rhs.end());

  for (int i = 0; i < 4; ++i)
  {
    if (ids_lhs[i] < ids_rhs[i])
      return true;
    if (ids_lhs[i] > ids_rhs[i])
      return false;
  }
  return false;
}

void Hexoworld::HexagonGrid::BorderRectangle::print_in_triList(
  std::vector<uint16_t>& TriList) const {
  Eigen::Vector3d a = Points::get_instance().get_point(AId);
  Eigen::Vector3d b = Points::get_instance().get_point(BId);
  Eigen::Vector3d c = Points::get_instance().get_point(CId);
  Eigen::Vector3d d = Points::get_instance().get_point(DId);

  int32_t heightstart =
    round(world.hexagonGrid_->heightDirection_.dot(
      a - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);
  int32_t heightend =
    round(world.hexagonGrid_->heightDirection_.dot(
      c - world.hexagonGrid_->origin_) /
      world.hexagonGrid_->heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(b, d);
    std::swap(heightstart, heightend);
  }

  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> stairs;
  stairs.push_back({ a, b });
  if (heightstart + 1 == heightend)
  {
    Eigen::Vector3d nell_vect(0, 0, 0);
    uint32_t nTerraces =
      (heightend - heightstart) *
      (world.hexagonGrid_->nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c - a) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      world.hexagonGrid_->heightDirection_
      .cross(step1)
      .cross(world.hexagonGrid_->heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;
    Eigen::Vector3d step2 = (d - b) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      world.hexagonGrid_->heightDirection_
      .cross(step2)
      .cross(world.hexagonGrid_->heightDirection_)
      .normalized();
    platformVector2 *= step2.dot(platformVector2) / 3;
    for (int i = 1; i <= nTerraces; ++i)
    {
      double len = (a - c).norm();
      Eigen::Vector3d p1 = a + step1 * i - 
        (terraces_side & 1 ? platformVector1 : nell_vect);
      Eigen::Vector3d p2 = b + step2 * i - 
        (terraces_side & 2 ? platformVector2 : nell_vect);
      Eigen::Vector3d p3 = a + step1 * i + 
        (terraces_side & 1 ? platformVector1 : nell_vect);
      Eigen::Vector3d p4 = b + step2 * i + 
        (terraces_side & 2 ? platformVector2 : nell_vect);

      stairs.push_back({ p1, p2 });
      stairs.push_back({ p3, p4 });
    }
  }
  stairs.push_back({ c, d });

  for (int i = 0; i < stairs.size() - 1; ++i)
    printRect(stairs[i], stairs[i + 1], TriList);
}

//struct HexagonGrid---------------------------------------
Hexoworld::HexagonGrid::HexagonGrid(
  Hexoworld& hexoworld, 
  float size, Eigen::Vector3d origin,
  Eigen::Vector3d row_direction, Eigen::Vector3d  col_direction,
  float height_step, uint32_t n_terraces_on_height_step)
  : origin_(origin), size_(size), innerSize_(0.75 * size),
  heightStep_(height_step),
  nTerracesOnHeightStep_(n_terraces_on_height_step),
  world(hexoworld)
{
  if (abs(col_direction.dot(row_direction)) > PRECISION_DBL_CALC)
    throw std::invalid_argument("ñol_direction and row_direction not perpendicular");

  row_direction.normalize();
  col_direction.normalize();

  rowDirection_ = row_direction;
  colDirection_ = col_direction;
  heightDirection_ = rowDirection_.cross(colDirection_);
}

std::pair<Hexoworld::HexagonGrid::Coord, 
  Hexoworld::HexagonGrid::Coord> 
  Hexoworld::HexagonGrid::pair_coords
  (Coord a, Coord b)
  {
    if (b < a)
      std::swap(a, b);
    return std::pair<Coord, Coord>(a, b);
  };

void Hexoworld::HexagonGrid::add_hexagon(uint32_t row, uint32_t col)
{
  if (grid_.find(Coord(row, col)) != grid_.end())
    return;

  std::shared_ptr<Hexagon> ptr =
    std::make_shared<Hexagon>(world, size_, innerSize_,
      origin_ +
      colDirection_ *
      ((sqrtf(3) * col + (row % 2) * sqrt(3) / 2) * size_)
      + rowDirection_ *
      (1.5 * size_ * row),
      rowDirection_, colDirection_,
      Coord(row, col));
  grid_[Coord(row, col)] = ptr;
  ptr->connect_points(ptr);

  for (int vertex = 0; vertex < 6; ++vertex)
  {
    uint32_t id = ptr->outerPointsId[vertex];
    std::vector<std::shared_ptr<Object>> objs =
      Points::get_instance().get_objects(id);
    std::vector<std::shared_ptr<Hexagon>> hexs;
    for (const auto& obj : objs)
      if (obj->is_hexagon())
        hexs.push_back(std::static_pointer_cast<Hexagon>(obj));

    //init triangles-------------------------------------
    std::vector<uint32_t> idInnerPoints;
    for (const auto& hex : hexs)
      for (int j = 0; j < 6; ++j)
        if (hex->outerPointsId[j] == id)
          idInnerPoints.push_back(hex->innerPointsId[j]);

    if (idInnerPoints.size() == 3)
    {
      triangles.insert(Triangle(world,
        idInnerPoints[0], idInnerPoints[1], idInnerPoints[2]));
    }

    //init rectangles-------------------------------------
    for (int i_hex1 = 0; i_hex1 < hexs.size(); ++i_hex1)
      for (int i_hex2 = i_hex1 + 1; i_hex2 < hexs.size(); ++i_hex2)
      {
        const auto& hex1 = hexs[i_hex1];
        const auto& hex2 = hexs[i_hex2];
        if (rectangles.find(
          pair_coords(hex1->coord, hex2->coord)) 
          == rectangles.end())
        {
          std::vector<uint32_t> hex1_inds_common_vertex;
          std::vector<uint32_t> hex2_inds_common_vertex;
          std::vector<uint32_t> vertex1_;
          std::vector<uint32_t> vertex2_;
          for (int vertex1 = 0; vertex1 < 6; vertex1++)
            for (int vertex2 = 0; vertex2 < 6; vertex2++)
            {
              if (hex1->outerPointsId[vertex1] ==
                hex2->outerPointsId[vertex2])
              {
                hex1_inds_common_vertex.push_back(
                  hex1->innerPointsId[vertex1]);
                hex2_inds_common_vertex.push_back(
                  hex2->innerPointsId[vertex2]);
                vertex1_.push_back(vertex1);
                vertex2_.push_back(vertex2);
              }
            }


          uint32_t ep_ind1 = hex1->get_ind_extraPoints(
            vertex1_[0],
            vertex1_[1]);
          uint32_t ep_ind2 = hex2->get_ind_extraPoints(
            vertex2_[0],
            vertex2_[1]);

          bool flag = vertex2_[1] < vertex2_[0];

          std::vector<std::pair<uint32_t, uint32_t>> lines;
          lines.push_back({ hex1_inds_common_vertex[0] ,
              hex2_inds_common_vertex[0] });
          for (uint32_t i = 0; i < hex1->extraPointsId[ep_ind1].size(); ++i)
          {
            lines.push_back({
              hex1->extraPointsId[ep_ind1][
                (flag ? i :
                  hex1->extraPointsId[ep_ind1].size() - i - 1)],
              hex2->extraPointsId[ep_ind2][
                (!flag ? i :
                hex2->extraPointsId[ep_ind2].size() - i - 1)]
              });
          }
          lines.push_back({ hex1_inds_common_vertex[1] ,
            hex2_inds_common_vertex[1] });

          for (int i = 0; i < lines.size() - 1; ++i)
            rectangles[pair_coords(hex1->coord, hex2->coord)]
            .push_back(BorderRectangle(world,
              lines[i].first,
              lines[i + 1].first,
              lines[i].second,
              lines[i + 1].second
            ));
        }
      }
  }
}

void add_point_in_vector(std::vector<Eigen::Vector3d>& dst, 
  const std::vector<Eigen::Vector3d>& src) {
  for (const Eigen::Vector3d& p : src)
    dst.push_back(p);
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::add_river(
  std::vector<std::pair<uint32_t, uint32_t>> hexs)
{
  std::vector<Coord> coords;
  for (int i = 0; i < hexs.size(); ++i)
    coords.push_back({ hexs[i].first, hexs[i].second });

  if (hexs.size() == 1)
    throw std::invalid_argument("so short river");
  if (grid_.find(coords[0]) == grid_.end())
    throw std::invalid_argument("Unknown hexagon");
  if (grid_.find(coords.back()) == grid_.end())
    throw std::invalid_argument("Unknown hexagon");

  std::vector<Eigen::Vector3d> points;

  add_point_in_vector(points, add_river_in_hex(
    Coord(-1, -1), coords[0], coords[1]));
  if (hexs.size() > 2){
    add_point_in_vector(points, add_river_in_hex(
    coords[coords.size() - 2], coords.back(), Coord(-1, -1)));
  }
  for (int i = 1; i < hexs.size() - 1; ++i)
  {
    if (grid_.find(Coord(hexs[i].first, hexs[i].second))
      == grid_.end())
      throw std::invalid_argument("Unknown hexagon");

    add_point_in_vector(points, add_river_in_hex(
    coords[i - 1], coords[i], coords[i + 1]));
  }

  return points;
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::add_river_in_hex(Coord before, Coord pos, Coord next) {
  if (before.row != -1 && next.row != -1)
  {
    rectangles.at(pair_coords(pos, before))[0]
      .set_terraces_side(1);
    rectangles.at(pair_coords(pos, before))[1]
      .set_terraces_side(0);
    rectangles.at(pair_coords(pos, before))[2]
      .set_terraces_side(0);
    rectangles.at(pair_coords(pos, before))[3]
      .set_terraces_side(2);

    rectangles.at(pair_coords(pos, next))[0]
      .set_terraces_side(1);
    rectangles.at(pair_coords(pos, next))[1]
      .set_terraces_side(0);
    rectangles.at(pair_coords(pos, next))[2]
      .set_terraces_side(0);
    rectangles.at(pair_coords(pos, next))[3]
      .set_terraces_side(2);
  }
  
  uint32_t ind_direction_before;
  if (before.row != -1)
    ind_direction_before = get_ind_direction(pos, before);
  else
    ind_direction_before = -1;

  uint32_t ind_direction_next;
  if (next.row != -1)
    ind_direction_next = get_ind_direction(pos, next);
  else
    ind_direction_next = -1;

  return grid_.at(pos)->
    make_river(ind_direction_before, ind_direction_next);
}
void Hexoworld::HexagonGrid::print_in_vertices_and_triList(
  std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) const
{
  for (const auto& [coord, hex] : grid_)
    hex->print_in_triList(TriList);
  for (const auto& triangle : triangles)
    triangle.print_in_triList(TriList);
  for (const auto& [pos, rects]:rectangles)
    for (const auto& rect : rects)
      rect.print_in_triList(TriList);

  Points::get_instance().print_in_vertices(Vertices);
}
void Hexoworld::HexagonGrid::set_height(int row, int col, int32_t height)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  grid_.at(Coord(row, col))->set_height(height);
}
std::vector<Eigen::Vector3d> Hexoworld::HexagonGrid::get_hex_points(int row, int col)
{
  if (grid_.find(Coord(row, col)) == grid_.end())
    throw std::invalid_argument("wrong coords");

  std::vector<Eigen::Vector3d> ans = grid_.at(Coord(row, col))->get_points();
  return ans;
}
uint32_t Hexoworld::HexagonGrid::get_ind_direction(Coord a, Coord b) {
  int32_t row_diff = b.row - a.row;
  int32_t col_diff = b.col - a.col;

  if (row_diff == 0)
    return (col_diff == 1 ? 1 : 4);
  
  if (a.row % 2 == 0)
  {
    if (row_diff == 1)
      return (col_diff == 0 ? 0 : 5);
    if (row_diff == -1)
      return (col_diff == 0 ? 2 : 3);
  }
  else
  {
    if (row_diff == 1)
      return (col_diff == 0 ? 5 : 0);
    if (row_diff == -1)
      return (col_diff == 0 ? 3: 2);
  }
}
