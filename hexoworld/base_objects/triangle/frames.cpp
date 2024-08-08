#include <hexoworld/base_objects/triangle/triangle.hpp>

Hexoworld::Triangle::TriangleFrame::TriangleFrame(Object* base)
  : Frame(base)
{}

Hexoworld::Triangle::UsualFrame::UsualFrame(Object* base, uint32_t AId, uint32_t BId, uint32_t CId)
  : TriangleFrame(base)
{
  if (static_cast<Triangle*>(base)->mainData == nullptr)
  {
    std::shared_ptr<MainData> mainData = 
      (static_cast<Triangle*>(base)->mainData = std::make_shared<MainData>());

    mainData->AId = AId;
    mainData->BId = BId;
    mainData->CId = CId;

    Eigen::Vector3d a = Points::get_instance().get_point(AId);
    Eigen::Vector3d b = Points::get_instance().get_point(BId);
    Eigen::Vector3d c = Points::get_instance().get_point(CId);

    int32_t heightA =
      round(base->world.heightDirection_.dot(
        a - base->world.origin_) /
        base->world.heightStep_);
    int32_t heightB =
      round(base->world.heightDirection_.dot(
        b - base->world.origin_) /
        base->world.heightStep_);
    int32_t heightC =
      round(base->world.heightDirection_.dot(
        c - base->world.origin_) /
        base->world.heightStep_);

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
      mainData->type = Smooth;
      mainData->middle_triangle.push_back(
        Points::get_instance().get_id_point(points[0].second, base)
      );
      mainData->middle_triangle.push_back(
        Points::get_instance().get_id_point(points[1].second, base)
      );
      mainData->middle_triangle.push_back(
        Points::get_instance().get_id_point(points[2].second, base)
      );
    }
    else
    {
      if (points[0].first != points[1].first &&
        points[1].first != points[2].first)
      {
        mainData->type = DiffHeight;
        uint32_t nTerraces =
          (points[2].first - points[0].first) *
          (base->world.nTerracesOnHeightStep_ + 1) - 1;
        Eigen::Vector3d step =
          (points[2].second -
            points[0].second) / (nTerraces + 1);
        Eigen::Vector3d platformVector =
          base->world.heightDirection_
          .cross(step)
          .cross(base->world.heightDirection_)
          .normalized();
        platformVector *= step.dot(platformVector) / 3;

        Eigen::Vector3d goal = points[0].second +
          step * (base->world.nTerracesOnHeightStep_ + 1)
          * (points[1].first - points[0].first);

        double len = (points[0].second
          - points[2].second).norm();

        Eigen::Vector3d p1;
        p1 = goal - platformVector;

        Eigen::Vector3d p2;
        p2 = goal + platformVector;


        init_stair(points[1].second, p1, points[0].second,
          points[1].second, goal, points[0].second, mainData->stairs_down,
          (points[0].first + 1 == points[1].first ? 2 : 3));
        init_stair(points[1].second, p2, points[2].second,
          points[1].second, goal, points[2].second, mainData->stairs_up,
          (points[1].first + 1 == points[2].first ? 2 : 3));

        mainData->middle_triangle.push_back(
          Points::get_instance().get_id_point(points[1].second, base)
        );
        mainData->middle_triangle.push_back(
          Points::get_instance().get_id_point(p1, base)
        );
        mainData->middle_triangle.push_back(
          Points::get_instance().get_id_point(p2, base)
        );
      }
      else
      {
        if (points[0].first == points[1].first)
        {
          mainData->type = TwoDown;
          init_stair(points[0].second,
            points[1].second,
            points[2].second,
            points[0].second,
            points[1].second,
            points[2].second,
            mainData->stairs_up,
            (points[0].first + 1 == points[2].first ? 0 : 3));
        }
        else
        {
          mainData->type = TwoUp;
          init_stair(points[1].second,
            points[2].second,
            points[0].second,
            points[1].second,
            points[2].second,
            points[0].second,
            mainData->stairs_down,
            (points[0].first + 1 == points[2].first ? 0 : 3));
        }
      }
    }
  }
}

std::vector<uint32_t> Hexoworld::Triangle::UsualFrame::get_pointsId() const
{
  std::shared_ptr<MainData> mainData = static_cast<Triangle*>(base)->mainData;

  std::set<uint32_t> ids = {
    mainData->AId,
    mainData->BId,
    mainData->CId
  };
  for (const auto& stair : mainData->stairs_up)
  {
    ids.insert(stair.first);
    ids.insert(stair.second);
  }
  for (const auto& stair : mainData->stairs_down)
  {
    ids.insert(stair.first);
    ids.insert(stair.second);
  }
  for (const auto& id : mainData->middle_triangle)
  {
    ids.insert(id);
  }

  std::vector<uint32_t> answer;
  answer.reserve(ids.size());
  for (uint32_t id : ids)
    answer.push_back(id);
  return answer;
}
std::vector<Eigen::Vector3d> Hexoworld::Triangle::UsualFrame::get_points() const
{
  std::vector<Eigen::Vector3d> points;
  for (uint32_t id : get_pointsId())
    points.push_back(Points::get_instance().get_point(id));

  return points;
}

void Hexoworld::Triangle::UsualFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  std::shared_ptr<MainData> mainData = static_cast<Triangle*>(base)->mainData;

  if (mainData->middle_triangle.size() == 3)
    printTri(
      mainData->middle_triangle[0],
      mainData->middle_triangle[1],
      mainData->middle_triangle[2],
      TriList);

  if (!mainData->stairs_up.empty())
    for (int i = 0; i < mainData->stairs_up.size() - 1; ++i)
      printRect(mainData->stairs_up[i], mainData->stairs_up[i + 1], TriList);

  if (!mainData->stairs_down.empty())
    for (int i = 0; i < mainData->stairs_down.size() - 1; ++i)
      printRect(mainData->stairs_down[i], mainData->stairs_down[i + 1], TriList);
}

void Hexoworld::Triangle::UsualFrame::init_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c, Eigen::Vector3d a_goal, Eigen::Vector3d b_goal, Eigen::Vector3d c_goal, std::vector<std::pair<uint32_t, uint32_t>>& stairs, uint32_t cliff)
{
  if ((cliff & 1) > 0)
    a = a_goal;
  if ((cliff & 2) > 0)
    b = b_goal;
  Eigen::Vector3d d = c;
  Eigen::Vector3d d_goal = c_goal;

  uint32_t a_id = Points::get_instance().get_id_point(a, base);
  uint32_t b_id = Points::get_instance().get_id_point(b, base);
  uint32_t c_id = Points::get_instance().get_id_point(c, base);
  uint32_t d_id = Points::get_instance().get_id_point(d, base);

  int32_t heightstart =
    round(base->world.heightDirection_.dot(
      a_goal - base->world.origin_) /
      base->world.heightStep_);
  int32_t heightend =
    round(base->world.heightDirection_.dot(
      c_goal - base->world.origin_) /
      base->world.heightStep_);

  stairs.push_back({ a_id, b_id });
  if (heightstart != heightend)
  {
    uint32_t nTerraces =
      abs(heightend - heightstart) *
      (base->world.nTerracesOnHeightStep_ + 1) - 1;

    Eigen::Vector3d step1, platformVector1;
    if ((cliff & 1) == 0)
    {
      step1 = (c_goal - a_goal) / (nTerraces + 1);
      platformVector1 = base->world.heightDirection_
        .cross(step1)
        .cross(base->world.heightDirection_)
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
      platformVector2 = base->world.heightDirection_
        .cross(step2)
        .cross(base->world.heightDirection_)
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

      stairs.push_back({
        Points::get_instance().get_id_point(p1, base),
        Points::get_instance().get_id_point(p2, base) });
      stairs.push_back({
        Points::get_instance().get_id_point(p3, base),
        Points::get_instance().get_id_point(p4, base) });
    }
  }
  stairs.push_back({ c_id, d_id });
}
