#include "hexagon.hpp"
#include "hexagon.hpp"
#include <hexoworld/base_objects/hexagon/hexagon.hpp>
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
    triangles_in_edges.resize(6, std::bitset<17>(((1 << 18) - 1)));
    static_cast<Hexagon*>(base)->mainData = std::make_shared<MainData>();

    std::random_device rd;
    static_cast<Hexagon*>(base)->mainData->gen_init = rd();

    Eigen::Vector3d center_; ///< центр шестиугольника
    std::vector<Eigen::Vector3d> polygonPoints_; ///< Точки шестиугольника.
    std::vector<std::vector<Eigen::Vector3d>> extraPoints_; ///< Дополнительные точки
    std::vector<std::vector<Eigen::Vector3d>> radial_points_;
    std::vector<std::vector<Eigen::Vector3d>> inner_points_;

    init_points(center, center_, polygonPoints_, extraPoints_, radial_points_, inner_points_);
#ifdef RANDOM
    random_move_points(center_, polygonPoints_, extraPoints_, radial_points_, inner_points_);
#endif
    init_ids(center_, polygonPoints_, extraPoints_, radial_points_, inner_points_);
  }
}

void Hexoworld::Hexagon::UsualFrame::set_height(int32_t height)
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  for (auto id : get_pointsId())
  {
    base->world.set_new_height_to_point(id, height, Usual);
  }
}

std::vector<Hexoworld::IdType> Hexoworld::Hexagon::UsualFrame::get_pointsId() const
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;
  std::vector<IdType> answerId;
  for (int i = 0; i < 6; ++i)
  {
    answerId.push_back(mainData->polygonPointsId[i]);

    for (int j = 0; j < 3; ++j)
    {
      answerId.push_back(mainData->extraPointsId[i][j]);
    }
  }
  answerId.push_back(mainData->centerId);
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 3; ++j)
      answerId.push_back(mainData->radial_points[i][j]);
    for (int j = 0; j < 4; ++j)
      answerId.push_back(mainData->inner_points[i][j]);
  }
  return answerId;
}
std::vector<Eigen::Vector3d> Hexoworld::Hexagon::UsualFrame::get_points() const
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  std::vector<Eigen::Vector3d> answer;
  for (const auto& i : get_pointsId())
    answer.push_back(
      {
        Points::get_instance().get_point(i)
      }
  );

  return answer;
}

void Hexoworld::Hexagon::UsualFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  for (int i = 0; i < 6; ++i)
  {
    {
      {
        if (triangles_in_edges[i][0])
          printTri(
            mainData->centerId,
            mainData->radial_points[i][0],
            mainData->inner_points[i][0], TriList);
        if (triangles_in_edges[i][1])
          printTri(
            mainData->centerId,
            mainData->radial_points[(i + 1) % 6][0],
            mainData->inner_points[i][0], TriList);
      }

      {
        if (triangles_in_edges[i][2])
          printTri(
          mainData->radial_points[i][0],
          mainData->inner_points[i][0],
          mainData->radial_points[i][1],
          TriList
        );
        if (triangles_in_edges[i][3])
          printTri(
          mainData->radial_points[(i + 1) % 6][0],
          mainData->inner_points[i][0],
          mainData->radial_points[(i + 1) % 6][1],
          TriList
        );
      }

      if (triangles_in_edges[i][4])
        printTri(
        mainData->radial_points[i][1],
        mainData->inner_points[i][0],
        mainData->radial_points[(i + 1) % 6][1],
        TriList
      );
    }

    {
      {
        if (triangles_in_edges[i][5])
          printTri(
          mainData->radial_points[i][1],
          mainData->radial_points[i][2],
          mainData->inner_points[i][1],
          TriList
        );
        if (triangles_in_edges[i][6])
          printTri(
          mainData->radial_points[(i + 1) % 6][1],
          mainData->radial_points[(i + 1) % 6][2],
          mainData->inner_points[i][3],
          TriList
        );
      }

       {
        if (triangles_in_edges[i][7])
          printTri(
          mainData->radial_points[i][1],
          mainData->inner_points[i][1],
          mainData->inner_points[i][2],
          TriList
        );
        if (triangles_in_edges[i][8])
          printTri(
          mainData->radial_points[(i + 1) % 6][1],
          mainData->inner_points[i][3],
          mainData->inner_points[i][2],
          TriList
        );
      }
       if (triangles_in_edges[i][9])
         printTri(
        mainData->radial_points[i][1],
        mainData->inner_points[i][2],
        mainData->radial_points[(i + 1) % 6][1],
        TriList
      );
    }

    {
      {
        if (triangles_in_edges[i][10])
          printTri(
          mainData->radial_points[i][2],
          mainData->polygonPointsId[i],
          mainData->extraPointsId[i][0],
          TriList
        );
        if (triangles_in_edges[i][11])
          printTri(
          mainData->radial_points[(i + 1) % 6][2],
          mainData->polygonPointsId[(i + 1) % 6],
          mainData->extraPointsId[i][2],
          TriList
        );
      }
      {
        if (triangles_in_edges[i][12])
          printTri(
          mainData->radial_points[i][2],
          mainData->inner_points[i][1],
          mainData->extraPointsId[i][0],
          TriList
        );
        if (triangles_in_edges[i][13])
          printTri(
          mainData->radial_points[(i + 1) % 6][2],
          mainData->inner_points[i][3],
          mainData->extraPointsId[i][2],
          TriList
        );
      }
      {
        if (triangles_in_edges[i][14])
          printTri(
          mainData->inner_points[i][1],
          mainData->inner_points[i][2],
          mainData->extraPointsId[i][0],
          TriList
        );
        if (triangles_in_edges[i][15])
          printTri(
          mainData->inner_points[i][3],
          mainData->inner_points[i][2],
          mainData->extraPointsId[i][2],
          TriList
        );
      }
      if (triangles_in_edges[i][16])
        printTri(
        mainData->inner_points[i][2],
        mainData->extraPointsId[i][0],
        mainData->extraPointsId[i][2],
        TriList
      );
    }
  }
}

void Hexoworld::Hexagon::UsualFrame::hide_triangle(int ind_edge, int ind_tr)
{
  triangles_in_edges[ind_edge][ind_tr] = 0;
}

void Hexoworld::Hexagon::UsualFrame::show_triangle(int ind_edge, int ind_tr)
{
  triangles_in_edges[ind_edge][ind_tr] = 1;
}


void Hexoworld::Hexagon::UsualFrame::init_points(Eigen::Vector3d center,
  Eigen::Vector3d& center_,
  std::vector<Eigen::Vector3d>& polygonPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& extraPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& radial_points_,
  std::vector<std::vector<Eigen::Vector3d>>& inner_points_)
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  Eigen::Vector3d dop = base->world.dop_height(Usual);
  center_ = center + dop;

  const Eigen::Vector3d& pointDirection = base->world.rowDirection_;
  const Eigen::Vector3d& floatDirection = base->world.colDirection_;

  polygonPoints_.resize(6);
  {
    polygonPoints_[0] = center_ + 
      (pointDirection * base->world.size_);

    polygonPoints_[1] = center_ + 
      (pointDirection * (0.5 * base->world.size_)) +
      (floatDirection * (sqrtf(3) / 2 * base->world.size_));

    polygonPoints_[2] = center_ - 
      (pointDirection * (0.5 * base->world.size_)) +
      (floatDirection * (sqrtf(3) / 2 * base->world.size_));

    polygonPoints_[3] = center_ - 
      (pointDirection * base->world.size_);

    polygonPoints_[4] = center_ - 
      (pointDirection * (0.5 * base->world.size_)) -
      (floatDirection * (sqrtf(3) / 2 * base->world.size_));

    polygonPoints_[5] = center_ + 
      (pointDirection * (0.5 * base->world.size_)) -
      (floatDirection * (sqrtf(3) / 2 * base->world.size_));
  }
  extraPoints_.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d a = polygonPoints_[i];
    Eigen::Vector3d b = polygonPoints_[(i + 1) % 6];
    Eigen::Vector3d v = (b - a) / 4;
    extraPoints_[i].resize(3);
    for (int j = 0; j < 3; ++j)
      extraPoints_[i][j] = a + v * (j + 1);
  }

  radial_points_.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    radial_points_[i].resize(3);
    Eigen::Vector3d vec = polygonPoints_[i] - center_;
    for (int j = 0; j < 3; ++j)
      radial_points_[i][j] = center_ + (j + 1) * vec / 4;
  }

  inner_points_.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    inner_points_[i].resize(4);
    {
      Eigen::Vector3d A = radial_points_[i][1];
      Eigen::Vector3d B = radial_points_[(i + 1) % 6][1];
      Eigen::Vector3d C = radial_points_[(i + 1) % 6][0];
      Eigen::Vector3d D = radial_points_[i][0];
      Eigen::Vector3d E = (A + B) / 2;
      Eigen::Vector3d F = (D + C) / 2;

      inner_points_[i][0] = E + 2 * (F - E) / 3;
    }
    inner_points_[i][1] = (radial_points_[i][1] + extraPoints_[i][0]) / 2;
    inner_points_[i][3] = (radial_points_[(i + 1) % 6][1] + extraPoints_[i][2]) / 2;
    inner_points_[i][2] = (inner_points_[i][1] + inner_points_[i][3]) / 2;
  }
}

void Hexoworld::Hexagon::UsualFrame::random_move_points(Eigen::Vector3d& center_,
  std::vector<Eigen::Vector3d>& polygonPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& extraPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& radial_points_,
  std::vector<std::vector<Eigen::Vector3d>>& inner_points_)
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  std::mt19937 gen(mainData->gen_init);
  std::uniform_real_distribution<> dis(-0.3, 0.3);
  double height_move = dis(gen);
  auto modify = [&dis, &gen, &height_move, this](Eigen::Vector3d& point) -> void
    {
      point +=
        base->world.rowDirection_ * dis(gen) +
        base->world.colDirection_ * dis(gen) +
        base->world.heightDirection_ * height_move;
    };

  bool correct_random = false;
  Eigen::Vector3d new_center_ = center_;
  std::vector<Eigen::Vector3d> new_points = polygonPoints_;
  std::vector<std::vector<Eigen::Vector3d>> new_extra = extraPoints_;
  while (!correct_random)
  {
    modify(new_center_);
    for (Eigen::Vector3d& point : new_points)
      modify(point);

    for (uint32_t i = 0; i < 6; ++i)
    {
      for (uint32_t j = 0; j <= 2; j += 2)
        modify(new_extra[i][j]);

      new_extra[i][1] = (new_extra[i][0] + new_extra[i][2]) / 2;
    }

    std::vector<Eigen::Vector3d> all_new_points;
    for (int i = 0; i < 6; ++i)
    {
      all_new_points.push_back(new_points[i]);
      all_new_points.push_back(new_extra[i][0]);
      all_new_points.push_back(new_extra[i][1]);
      all_new_points.push_back(new_extra[i][2]);
    }

    double cross_dot = (all_new_points[0] - new_center_).cross(
      all_new_points[1] - center_).dot(base->world.heightDirection_);
    int sign_cross_dot = round(cross_dot / abs(cross_dot));
    correct_random = true;

    for (int i = 1; i < all_new_points.size(); ++i)
    {
      cross_dot = (all_new_points[i] - new_center_).cross(
        all_new_points[(i + 1) % all_new_points.size()] - center_).dot(
          base->world.heightDirection_);

      if (sign_cross_dot != round(cross_dot / abs(cross_dot)))
      {
        correct_random = false;
        new_center_ = center_;
        new_points = polygonPoints_;
        new_extra = extraPoints_;
        break;
      }
    }
  }

  std::swap(center_, new_center_);
  std::swap(new_points, polygonPoints_);
  std::swap(new_extra, extraPoints_);

  std::uniform_real_distribution<> dis_height_inner_points(-0.1, 0.1);
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 3; ++j)
      radial_points_[i][j] += dis_height_inner_points(gen) * 
      base->world.heightDirection_;
    
    for (int j = 0; j < 4; ++j)
      inner_points_[i][j] += dis_height_inner_points(gen) *
      base->world.heightDirection_;
  }
}

void Hexoworld::Hexagon::UsualFrame::init_ids(Eigen::Vector3d& center_,
  std::vector<Eigen::Vector3d>& polygonPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& extraPoints_,
  std::vector<std::vector<Eigen::Vector3d>>& radial_points_,
  std::vector<std::vector<Eigen::Vector3d>>& inner_points_)
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  mainData->centerId = Points::get_instance().get_id_point(center_, base);

  mainData->polygonPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
    mainData->polygonPointsId[i] = Points::get_instance().get_id_point(
      polygonPoints_[i], base);

  mainData->extraPointsId.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    mainData->extraPointsId[i].resize(extraPoints_[i].size());

    for (int j = 0; j < extraPoints_[i].size(); ++j)
      mainData->extraPointsId[i][j] = Points::get_instance().get_id_point(
        extraPoints_[i][j], base);
  }

  mainData->radial_points.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    mainData->radial_points[i].resize(radial_points_[i].size());
    for (int j = 0; j < radial_points_[i].size(); ++j)
      mainData->radial_points[i][j] = Points::get_instance().get_id_point(
        radial_points_[i][j], base);
  }
  mainData->inner_points.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    mainData->inner_points[i].resize(4);
    for (int j = 0; j < 4; ++j)
      mainData->inner_points[i][j] = Points::get_instance().get_id_point(
        inner_points_[i][j], base);
  }
}

//struct RiversType----------------------------------------

Hexoworld::Hexagon::RiversFrame::RiversFrame(Object* base,
  int32_t in, int32_t out, double deep)
  : HexagonFrame(base), in_(in), out_(out), deep_(deep)
{
  std::shared_ptr<MainData> mainData = static_cast<Hexagon*>(base)->mainData;

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

  Eigen::Vector3d dop = base->world.dop_height(River);

  radial_points.resize(6, std::vector<IdType>(3));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (polygonPoints[i] - center) / 4;
    radial_points[i][0] = Points::get_instance().get_id_point(center + 1 * v + dop, this);
    radial_points[i][1] = Points::get_instance().get_id_point(center + 2 * v + dop, this);
    radial_points[i][2] = Points::get_instance().get_id_point(center + 3 * v + dop, this);
  }

  middle_points.resize(6, std::vector<IdType>(2));
  for (int i = 0; i < 6; ++i)
  {
    Eigen::Vector3d v = (extraPoints[i][1] - center) / 4;
    middle_points[i][0] = Points::get_instance().get_id_point(center + 1 * v + dop, this);
    middle_points[i][1] = Points::get_instance().get_id_point(center + 2 * v + dop, this);
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
  std::vector<Eigen::Vector3d> answer;
  
  for (IdType i : get_pointsId())
    answer.push_back(
      Points::get_instance().get_point(i)
    );

  return answer;
}

std::vector<Hexoworld::IdType> Hexoworld::Hexagon::RiversFrame::get_pointsId() const
{
  std::vector<IdType> ans;

  for (const IdType& point : shore_points)
    ans.push_back(point);
  for (const IdType& point : floor_points)
    ans.push_back(point);
  for (const IdType& point : water_points)
    ans.push_back(point);

  return ans;
}
std::vector<Hexoworld::IdType> Hexoworld::Hexagon::RiversFrame::get_floor_pointsId() const
{
  return floor_points;
}
std::vector<Hexoworld::IdType> Hexoworld::Hexagon::RiversFrame::get_shore_pointsId() const
{
  return shore_points;
}
std::vector<Hexoworld::IdType> Hexoworld::Hexagon::RiversFrame::get_water_pointsId() const
{
  return water_points;
}

void Hexoworld::Hexagon::RiversFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (int i = 0; i < shore_points.size() - 2; i++)
  {
    printRect(
      { shore_points[i], shore_points[i + 2] },
      { water_points[i / 2], water_points[i / 2 + 1] },
      TriList);
  }
}

void Hexoworld::Hexagon::RiversFrame::make_river_begin_end(uint32_t edge)
{
  Eigen::Vector3d dop = base->world.dop_height(River);

  floor_points.push_back(middle_points[edge][1]);
  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[edge][1]
      ) + dop, 
      this
    )
  );

  shore_points.push_back(middle_points[edge][0]);
  shore_points.push_back(middle_points[edge][0]);
  water_points.push_back(middle_points[edge][0]);

  shore_points.push_back(radial_points[(edge + 1) % 6][1]);
  shore_points.push_back(radial_points[edge][1]);
  water_points.push_back(middle_points[edge][1]);

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[edge][2]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[edge][0]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[edge][1]
      ) + dop,
      this
    )
  );
}
void Hexoworld::Hexagon::RiversFrame::make_river_directly(uint32_t in, uint32_t out)
{
  Eigen::Vector3d dop = base->world.dop_height(River);

  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );
  floor_points.push_back(middle_points[in][1]);
  floor_points.push_back(middle_points[in][0]);
  floor_points.push_back(radial_points[in][0]);
  floor_points.push_back(radial_points[(in + 1) % 6][0]);
  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->centerId
      ) + dop,
      this
    )
  );
  floor_points.push_back(radial_points[out][0]);
  floor_points.push_back(radial_points[(out + 1) % 6][0]);
  floor_points.push_back(middle_points[out][0]);
  floor_points.push_back(middle_points[out][1]);
  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][0]
      ) + dop,
      this
    )
  );
  shore_points.push_back(Points::get_instance().get_id_point(
    Points::get_instance().get_point(
      static_cast<Hexagon*>(base)->mainData->extraPointsId[in][2]
    ) + dop,
    this
  )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][2]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][0]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_2(uint32_t in, uint32_t out)
{
  Eigen::Vector3d dop = base->world.dop_height(River);

  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );
  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );

  floor_points.push_back(middle_points[in][0]);
  floor_points.push_back(middle_points[in][1]);
  floor_points.push_back(middle_points[out][0]);
  floor_points.push_back(middle_points[out][1]);

  uint32_t middle = (in + 1) % 6;
  floor_points.push_back(radial_points[middle][0]);
  floor_points.push_back(radial_points[out][0]);

  floor_points.push_back(middle_points[middle][0]);

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][0]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][2]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );

  shore_points.push_back(radial_points[in][1]);
  shore_points.push_back(radial_points[(in + 1) % 6][1]);
  water_points.push_back(middle_points[in][1]);

  shore_points.push_back(radial_points[(out + 1) % 6][1]);
  shore_points.push_back(radial_points[out][1]);
  water_points.push_back(middle_points[out][1]);

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][2]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][0]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );
}
void Hexoworld::Hexagon::RiversFrame::make_river_angle_1(uint32_t in, uint32_t out)
{
  Eigen::Vector3d dop = base->world.dop_height(River);

  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );
  floor_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );

  floor_points.push_back(middle_points[in][1]);
  floor_points.push_back(middle_points[out][1]);

  floor_points.push_back(middle_points[in][0]);
  floor_points.push_back(middle_points[out][0]);

  floor_points.push_back(radial_points[out][1]);
  floor_points.push_back(radial_points[out][0]);

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][0]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][2]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[in][1]
      ) + dop,
      this
    )
  );

  shore_points.push_back(radial_points[out][0]);
  shore_points.push_back(radial_points[out][2]);
  water_points.push_back(radial_points[out][1]);

  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][2]
      ) + dop,
      this
    )
  );
  shore_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][0]
      ) + dop,
      this
    )
  );
  water_points.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[out][1]
      ) + dop,
      this
    )
  );
}

//struct FloodFrame----------------------------------------

Hexoworld::Hexagon::FloodFrame::FloodFrame(Object* base, double water_level) 
  : HexagonFrame(base), water_level(water_level)
{
  for (int i = 0; i < 6; ++i)
  {
    waterPoints.push_back(
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(
          static_cast<Hexagon*>(base)->mainData->polygonPointsId[i]
        ) + base->world.heightDirection_ * water_level,
        this
      )
    );
    for (int j = 0; j < static_cast<Hexagon*>(base)->mainData->extraPointsId[i].size(); ++j)
      waterPoints.push_back(
        Points::get_instance().get_id_point(
          Points::get_instance().get_point(
            static_cast<Hexagon*>(base)->mainData->extraPointsId[i][j]
          ) + base->world.heightDirection_ * water_level,
          this
        )
      );
  }
  waterPoints.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->centerId
      ) + base->world.heightDirection_ * water_level,
      this
    )
  );
}

void Hexoworld::Hexagon::FloodFrame::set_height(int32_t height)
{
  for (auto& wpId : waterPoints)
  {
    Eigen::Vector3d p = Points::get_instance().get_point(wpId);
    
    p -= base->world.heightDirection_ * water_level;
    base->world.set_new_height_to_point(p, height, Flood);
    p += base->world.heightDirection_ * water_level;

    Points::get_instance().update_point(wpId, p);
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::FloodFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;

  for (IdType i : get_pointsId())
    answer.push_back(
      Points::get_instance().get_point(i)
    );

  return answer;
}

std::vector<Hexoworld::IdType> Hexoworld::Hexagon::FloodFrame::get_pointsId() const
{
  std::vector<IdType> answer;
  for (const auto& p : waterPoints)
    answer.push_back(p);

  return answer;
}

void Hexoworld::Hexagon::FloodFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (int i = 0; i < waterPoints.size() - 1; ++i)
    printTri(waterPoints.back(),
      waterPoints[i],
      waterPoints[(i + 1) % (waterPoints.size() - 1)],
      TriList);
}

//struct RoadFrame-----------------------------------------

Hexoworld::Hexagon::RoadFrame::RoadFrame(Object* object, std::vector<uint32_t> edges)
  : HexagonFrame(object)
{
  auto mainData = static_cast<Hexagon*>(base)->mainData;

  for (int i = 0; i < 6; ++i)
  {
    std::static_pointer_cast<UsualFrame>(base->frames[Usual])->hide_triangle(i, 0);
    std::static_pointer_cast<UsualFrame>(base->frames[Usual])->hide_triangle(i, 1);
  }

  centerId = Points::get_instance().get_id_point(
    Points::get_instance().get_point(
      static_cast<Hexagon*>(base)->mainData->centerId
    ) + base->world.dop_height(Road),
    this
  );

  crossroads.resize(6, IdType());
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
  middle_points.resize(6);
  for (int i = 0; i < 6; ++i)
  {
    middle_points[i] = Points::get_instance().get_id_point(
      center + (extraPoints[i][1] - center) / 3 + base->world.dop_height(Road), this);
  }
  for (uint32_t i = 0; i < 6; ++i)
    crossroads[i] = middle_points[i];

  isRoad.resize(6, false);
  for (uint32_t i : edges)
    add_road(i);
}

void Hexoworld::Hexagon::RoadFrame::add_road(uint32_t ind)
{
  if (!isRoad[ind])
  {
    isRoad[ind] = true;

    {
      auto hide = [ind, this](int id) -> void {
        std::static_pointer_cast<UsualFrame>
          (base->frames[Usual])->hide_triangle(ind, id);
        };
      
      hide(16);
      hide(15);
      hide(14);
      hide(8);
      hide(7);
      hide(9);
      hide(4);
      hide(3);
      hide(2);
      std::static_pointer_cast<UsualFrame>
        (base->frames[Usual])->hide_triangle((ind + 5) % 6, 3);
      std::static_pointer_cast<UsualFrame>
        (base->frames[Usual])->hide_triangle((ind + 1) % 6, 2);
    }

    auto mainData = static_cast<Hexagon*>(base)->mainData;

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

    radial_points.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      radial_points[i] = Points::get_instance().get_id_point(
        center + (polygonPoints[i] - center) / 2 + base->world.dop_height(Road), this
      );
    }
    middle_points.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      middle_points[i] = Points::get_instance().get_id_point(
        center + (extraPoints[i][1] - center) / 3 + base->world.dop_height(Road), this);
    }
    for (uint32_t i = 0; i < 6; ++i)
      crossroads[i] = middle_points[i];

    for (int i = 0; i < 6; i++) if (isRoad[i])
    {
      init_road(ind);

      if (isRoad[(ind + 1) % 6])
        endOfFence[(ind + 1) % 6].first = 
          Points::get_instance().get_id_point(
            cuts_intersection(
              Points::get_instance().get_point(inputs[ind].second),
              Points::get_instance().get_point(crossroads[(ind + 1) % 6]),
              Points::get_instance().get_point(inputs[(ind + 1) % 6].first),
              Points::get_instance().get_point(crossroads[ind])
            ),
            this
          );

      if (isRoad[(ind + 5) % 6])
        endOfFence[(ind + 5) % 6].second = 
          Points::get_instance().get_id_point(
            cuts_intersection(
              Points::get_instance().get_point(inputs[ind].first),
              Points::get_instance().get_point(crossroads[(ind + 5) % 6]),
              Points::get_instance().get_point(inputs[(ind + 5) % 6].second),
              Points::get_instance().get_point(crossroads[ind])
            ),
            this
          );
    }
  }
}

void Hexoworld::Hexagon::RoadFrame::del_road(uint32_t ind)
{
  if (isRoad[ind])
  {
    isRoad[ind] = false;

    {
      auto show = [ind, this](int id) -> void {
        std::static_pointer_cast<UsualFrame>
          (base->frames[Usual])->show_triangle(ind, id);
        };

      show(16);
      show(15);
      show(14);
      show(8);
      show(7);
      show(9);
      show(4);
      show(3);
      show(2);
      std::static_pointer_cast<UsualFrame>
        (base->frames[Usual])->show_triangle((ind + 5) % 6, 3);
      std::static_pointer_cast<UsualFrame>
        (base->frames[Usual])->show_triangle((ind + 1) % 6, 2);
    }

    middleFence.erase(ind);

    auto mainData = static_cast<Hexagon*>(base)->mainData;
//    std::static_pointer_cast<Hexagon::UsualFrame>(base->frames[Usual])->normalize_edge(ind);

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

    radial_points.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      radial_points[i] = Points::get_instance().get_id_point(
        center + (polygonPoints[i] - center) / 2 + base->world.dop_height(Road), this
      );
    }
    middle_points.resize(6);
    for (int i = 0; i < 6; ++i)
    {
      middle_points[i] = Points::get_instance().get_id_point(
        center + (extraPoints[i][1] - center) / 3 + base->world.dop_height(Road), this);
    }
    for (uint32_t i = 0; i < 6; ++i)
      crossroads[i] = middle_points[i];

    for (int i = 0; i < 6; i++) if (isRoad[i])
    {
      init_road(i);

      if (isRoad[(i + 1) % 6])
        endOfFence[(i + 1) % 6].first =
        Points::get_instance().get_id_point(
          cuts_intersection(
            Points::get_instance().get_point(inputs[i].second),
            Points::get_instance().get_point(crossroads[(i + 1) % 6]),
            Points::get_instance().get_point(inputs[(i + 1) % 6].first),
            Points::get_instance().get_point(crossroads[i])
          ),
          this
        );

      if (isRoad[(i + 5) % 6])
        endOfFence[(i + 5) % 6].second =
        Points::get_instance().get_id_point(
          cuts_intersection(
            Points::get_instance().get_point(inputs[i].first),
            Points::get_instance().get_point(crossroads[(i + 5) % 6]),
            Points::get_instance().get_point(inputs[(i + 5) % 6].second),
            Points::get_instance().get_point(crossroads[i])
          ),
          this
        );
    }
  }
}

void Hexoworld::Hexagon::RoadFrame::init_road(uint32_t ind)
{
  Eigen::Vector3d dop = base->world.dop_height(Road);
  inputs[ind] = {
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[ind][0]
      ) + dop,
      this
    ),
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[ind][2]
      ) + dop,
      this
    )
  };

  if (isRoad[(ind + 5) % 6])
    endOfFence[ind].first = 
      Points::get_instance().get_id_point(
        cuts_intersection(
          Points::get_instance().get_point(inputs[(ind + 5) % 6].second),
          Points::get_instance().get_point(crossroads[ind]),
          Points::get_instance().get_point(inputs[ind].first),
          Points::get_instance().get_point(crossroads[(ind + 5) % 6])
        ),
        this
      );
  else
    endOfFence[ind].first = crossroads[(ind + 5) % 6];

  if (isRoad[(ind + 1) % 6])
    endOfFence[ind].second = 
      Points::get_instance().get_id_point(
        cuts_intersection(
          Points::get_instance().get_point(inputs[(ind + 1) % 6].first),
          Points::get_instance().get_point(crossroads[ind]),
          Points::get_instance().get_point(inputs[ind].second),
          Points::get_instance().get_point(crossroads[(ind + 1) % 6])
        ),
        this
      );
  else
    endOfFence[ind].second = crossroads[(ind + 1) % 6];

  middleFence[ind] = {
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(
        static_cast<Hexagon*>(base)->mainData->extraPointsId[ind][1]
      ) + dop,
      this
    ),
    crossroads[ind]
  };
}

Eigen::Vector3d Hexoworld::Hexagon::RoadFrame::cuts_intersection(Eigen::Vector3d start1, Eigen::Vector3d end1, Eigen::Vector3d start2, Eigen::Vector3d end2)
{
  Eigen::Vector3d a = (end1 - start1).normalized();
  Eigen::Vector3d b = (end2 - start2).normalized();
  double len = a.cross(start2 - start1).norm();

  return start2 + len / sqrt(1 - pow(a.dot(b), 2)) * b;
}

void Hexoworld::Hexagon::RoadFrame::set_height(int32_t height)
{
  for (auto& p : radial_points)
    base->world.set_new_height_to_point(p, height, Road);
  for (auto& p : middle_points)
    base->world.set_new_height_to_point(p, height, Road);
  for (auto& p : crossroads)
    base->world.set_new_height_to_point(p, height, Road);

  for (auto& [ind, p] : inputs)
  {
    base->world.set_new_height_to_point(p.first, height, Road);
    base->world.set_new_height_to_point(p.second, height, Road);
  }
  for (auto& [ind, p] : endOfFence)
  {
    base->world.set_new_height_to_point(p.first, height, Road);
    base->world.set_new_height_to_point(p.second, height, Road);
  }
  for (auto& [ind, p] : middleFence)
  {
    base->world.set_new_height_to_point(p.first, height, Road);
    base->world.set_new_height_to_point(p.second, height, Road);
  }
  base->world.set_new_height_to_point(centerId, height, Road);
}

std::vector<Eigen::Vector3d> Hexoworld::Hexagon::RoadFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;

  for (IdType i : get_pointsId())
    answer.push_back(
      Points::get_instance().get_point(i)
    );

  return answer;
}

std::vector<Hexoworld::IdType> Hexoworld::Hexagon::RoadFrame::get_pointsId() const
{
  std::vector<IdType> answer = {centerId};

  for (auto& p : crossroads)
    answer.push_back(p);

  for (auto& [ind, p] : inputs)
  {
    answer.push_back(p.first);
    answer.push_back(p.second);
  }
  for (auto& [ind, p] : endOfFence)
  {
    answer.push_back(p.first);
    answer.push_back(p.second);
  }
  for (auto& [ind, p] : middleFence)
  {
    answer.push_back(p.first);
    answer.push_back(p.second);
  }

  return answer;
}

std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> Hexoworld::Hexagon::RoadFrame::get_inner_fence()
{
  std::vector<std::pair<IdType, IdType>> answerId;
  for (const auto& [ind, pp] : middleFence)
    answerId.push_back(pp);

  if (
    isRoad[0] +
    isRoad[1] +
    isRoad[2] +
    isRoad[3] +
    isRoad[4] +
    isRoad[5] == 2)
  {
    for (int i = 0; i < 6; ++i)
    {
      if (isRoad[i])
        answerId.push_back({ 
          crossroads[i],
          Points::get_instance().get_id_point(
            Points::get_instance().get_point(
              static_cast<Hexagon*>(base)->mainData->centerId
            ) + base->world.dop_height(Road),
            this
          ) 
        });
    }
  }
  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> answer;
  for (const auto& i : answerId)
    answer.push_back(
      {
        Points::get_instance().get_point(i.first),
        Points::get_instance().get_point(i.second)
      }
    );
  return answer;
}

std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> Hexoworld::Hexagon::RoadFrame::get_outer_fence()
{
  std::vector<std::pair<IdType, IdType>> answerId;

  for (int i = 0; i < 6; ++i) if (isRoad[i])
  {
    answerId.push_back({inputs[i].first, endOfFence[i].first});
    answerId.push_back({inputs[i].second, endOfFence[i].second});
  }

  for (int i = 0; i < 6; ++i) if (!isRoad[i] && !isRoad[(i + 1) % 6])
  {
    answerId.push_back({crossroads[i], crossroads[(i + 1) % 6]});
  }
  std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> answer;
  for (const auto& i : answerId)
    answer.push_back(
      {
        Points::get_instance().get_point(i.first),
        Points::get_instance().get_point(i.second)
      }
  );
  return answer;
}

void Hexoworld::Hexagon::RoadFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (int i = 0; i < 6; ++i)
    printTri(
      centerId,
      crossroads[i], 
      crossroads[(i + 1) % 6],
      TriList);

  for (uint32_t ind_road = 0; ind_road < 6; ++ind_road) if (isRoad[ind_road]) {
    printRect(
      {
        inputs.at(ind_road).first, middleFence.at(ind_road).first
      },
      {
        crossroads[(ind_road + 5) % 6],
        crossroads[ind_road]
      },
      TriList
    );
    printRect(
      {
        middleFence.at(ind_road).first,inputs.at(ind_road).second
      },
      {
        crossroads[ind_road],
        crossroads[(ind_road + 1) % 6]
      },
      TriList
    );
  }
}
Hexoworld::Hexagon::RoadFrame::~RoadFrame() {
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 17; ++j)
      std::static_pointer_cast<UsualFrame>(base->frames[Usual])->show_triangle(i, j);
  }
}