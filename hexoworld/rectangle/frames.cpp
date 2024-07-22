#include <hexoworld/hexoworld.hpp>


void Hexoworld::Rectangle::RectangleFrame::init_stairs(uint32_t AId, uint32_t BId,
  uint32_t CId, uint32_t DId, uint32_t terraces_side, std::vector<std::pair<uint32_t, uint32_t>>& stairs)
{
  Eigen::Vector3d a = Points::get_instance().get_point(AId);
  Eigen::Vector3d b = Points::get_instance().get_point(BId);
  Eigen::Vector3d c = Points::get_instance().get_point(CId);
  Eigen::Vector3d d = Points::get_instance().get_point(DId);
  uint32_t a_id = AId;
  uint32_t b_id = BId;
  uint32_t c_id = CId;
  uint32_t d_id = DId;

  int32_t heightstart =
    round(base->world.heightDirection_.dot(
      a - base->world.origin_) /
      base->world.heightStep_);
  int32_t heightend =
    round(base->world.heightDirection_.dot(
      c - base->world.origin_) /
      base->world.heightStep_);

  if (heightstart > heightend)
  {
    std::swap(a, c);
    std::swap(a_id, c_id);
    std::swap(b, d);
    std::swap(b_id, d_id);
    std::swap(heightstart, heightend);
  }


  stairs.push_back({ a_id, b_id });
  if (heightstart + 1 == heightend)
  {
    Eigen::Vector3d nell_vect(0, 0, 0);
    uint32_t nTerraces =
      (heightend - heightstart) *
      (base->world.nTerracesOnHeightStep_ + 1) - 1;
    Eigen::Vector3d step1 =
      (c - a) / (nTerraces + 1);
    Eigen::Vector3d platformVector1 =
      base->world.heightDirection_
      .cross(step1)
      .cross(base->world.heightDirection_)
      .normalized();
    platformVector1 *= step1.dot(platformVector1) / 3;
    Eigen::Vector3d step2 = (d - b) / (nTerraces + 1);
    Eigen::Vector3d platformVector2 =
      base->world.heightDirection_
      .cross(step2)
      .cross(base->world.heightDirection_)
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

Hexoworld::Rectangle::RectangleFrame::RectangleFrame(Object* base,
  uint32_t Aid, uint32_t Bid, uint32_t Cid, uint32_t Did,
  std::vector<uint32_t> ABids, std::vector<uint32_t> CDids,
  std::shared_ptr<MainData> _mainData)
  : Frame(base), mainData(_mainData)
{
  if (mainData == nullptr)
  {
    mainData = std::make_shared<MainData>();
    mainData->AId = Aid;
    mainData->BId = Bid;
    mainData->CId = Cid;
    mainData->DId = Did;
    mainData->ABIds = ABids;
    mainData->CDIds = CDids;

    std::vector<uint32_t> first_border = { Aid };
    for (uint32_t id : ABids)
      first_border.push_back(id);
    first_border.push_back(Bid);


    std::vector<uint32_t> second_border = { Cid };
    for (uint32_t id : CDids)
      second_border.push_back(id);
    second_border.push_back(Did);

    mainData->stairs.resize(first_border.size() - 1);
    for (int i = 0; i < first_border.size() - 1; ++i)
      init_stairs(
        first_border[i], first_border[i + 1], second_border[i], second_border[i + 1], 3, mainData->stairs[i]
      );
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Rectangle::RectangleFrame::get_points() const
{
  std::set<uint32_t> ids = {
    mainData->AId,
    mainData->BId,
    mainData->CId,
    mainData->DId,
  };

  for (const auto& id : mainData->ABIds)
    ids.insert(id);
  for (const auto& id : mainData->CDIds)
    ids.insert(id);
  for (int i = 0; i < mainData->stairs.size(); ++i)
    for (const auto& stair : mainData->stairs[i])
    {
      ids.insert(stair.first);
      ids.insert(stair.second);
    }

  std::vector<Eigen::Vector3d> points;
  for (uint32_t id : ids)
    points.push_back(Points::get_instance().get_point(id));

  return points;
}

void Hexoworld::Rectangle::RectangleFrame::print_in_triList(std::vector<uint16_t>& TriList) const {
  for (int j = 0; j < mainData->stairs.size(); ++j)
    for (int i = 0; i < mainData->stairs[j].size() - 1; ++i)
      printRect(mainData->stairs[j][i], mainData->stairs[j][i + 1], TriList);
}

Hexoworld::Rectangle::UsualFrame::UsualFrame(Object* base,
  uint32_t Aid, uint32_t Bid, uint32_t Cid, uint32_t Did,
  std::vector<uint32_t> ABids, std::vector<uint32_t> CDids,
  std::shared_ptr<MainData> mainData)
  : RectangleFrame(base, Aid, Bid, Cid, Did, ABids, CDids, mainData)
{}

Hexoworld::Rectangle::RiverFrame::RiverFrame(Object* base,
  uint32_t Aid, uint32_t Bid, uint32_t Cid, uint32_t Did,
  std::vector<uint32_t> ABids, std::vector<uint32_t> CDids,
  std::shared_ptr<MainData> mainData)
  : RectangleFrame(base, Aid, Bid, Cid, Did, ABids, CDids, mainData)
{
  Eigen::Vector3d a = Points::get_instance().get_point(ABids[0]);
  Eigen::Vector3d b = Points::get_instance().get_point(ABids[2]);
  Eigen::Vector3d c = Points::get_instance().get_point(CDids[0]);
  Eigen::Vector3d d = Points::get_instance().get_point(CDids[2]);

  waterPoints.push_back(a + (b - a) / 2);
  waterPoints.push_back(c + (d - c) / 2);
}

void Hexoworld::Rectangle::RiverFrame::print_in_triList(std::vector<uint16_t>& TriList) const
{
  RectangleFrame::print_in_triList(TriList);

  std::vector<uint32_t> waterPointsId;
  for (int i = 0; i < waterPoints.size(); ++i)
    waterPointsId.push_back(
      Points::get_instance().get_id_point(waterPoints[i], base)
    );

  printRect(
    { mainData->ABIds[0],  waterPointsId[0] },
    { mainData->CDIds[0], waterPointsId[1] },
    TriList
  );
  printRect(
    { mainData->ABIds[2],  waterPointsId[0] },
    { mainData->CDIds[2], waterPointsId[1] },
    TriList
  );
}