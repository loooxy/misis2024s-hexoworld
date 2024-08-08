#include <hexoworld/base_objects/rectangle/rectangle.hpp>


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

  bool was_reverse = false;
  if (heightstart > heightend)
  {
    was_reverse = true;
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

  if (was_reverse)
  {
    std::reverse(stairs.begin(), stairs.end());
  }
}

Hexoworld::Rectangle::RectangleFrame::RectangleFrame(Object* base)
  : Frame(base)
{}

void Hexoworld::Rectangle::RectangleFrame::delete_middle_terraces()
{
  std::vector<uint32_t> first_border = { static_cast<Rectangle*>(base)->mainData->AId };
  for (uint32_t id : static_cast<Rectangle*>(base)->mainData->ABIds)
    first_border.push_back(id);
  first_border.push_back(static_cast<Rectangle*>(base)->mainData->BId);


  std::vector<uint32_t> second_border = { static_cast<Rectangle*>(base)->mainData->CId };
  for (uint32_t id : static_cast<Rectangle*>(base)->mainData->CDIds)
    second_border.push_back(id);
  second_border.push_back(static_cast<Rectangle*>(base)->mainData->DId);

  static_cast<Rectangle*>(base)->mainData->stairs.clear();
  static_cast<Rectangle*>(base)->mainData->stairs.resize(first_border.size() - 1);
  init_stairs(
    first_border[0], first_border[1], second_border[0], second_border[1], 1,
    static_cast<Rectangle*>(base)->mainData->stairs[0]
  );

  for (int i = 1; i < first_border.size() - 2; ++i)
    init_stairs(
      first_border[i], first_border[i + 1], second_border[i], second_border[i + 1], 0,
      static_cast<Rectangle*>(base)->mainData->stairs[i]
    );

  init_stairs(
    first_border[first_border.size() - 2], first_border[first_border.size() - 1], 
    second_border[first_border.size() - 2], second_border[first_border.size() - 1], 
    2,
    static_cast<Rectangle*>(base)->mainData->stairs[first_border.size() - 2]
  );
}

Hexoworld::Rectangle::UsualFrame::UsualFrame(Object* base,
  uint32_t Aid, uint32_t Bid, uint32_t Cid, uint32_t Did,
  std::vector<uint32_t> ABids, std::vector<uint32_t> CDids)
  : RectangleFrame(base)
{
  if (static_cast<Rectangle*>(base)->mainData == nullptr)
  {
    static_cast<Rectangle*>(base)->mainData = std::make_shared<MainData>();
    static_cast<Rectangle*>(base)->mainData->AId = Aid;
    static_cast<Rectangle*>(base)->mainData->BId = Bid;
    static_cast<Rectangle*>(base)->mainData->CId = Cid;
    static_cast<Rectangle*>(base)->mainData->DId = Did;
    static_cast<Rectangle*>(base)->mainData->ABIds = ABids;
    static_cast<Rectangle*>(base)->mainData->CDIds = CDids;

    std::vector<uint32_t> first_border = { Aid };
    for (uint32_t id : ABids)
      first_border.push_back(id);
    first_border.push_back(Bid);


    std::vector<uint32_t> second_border = { Cid };
    for (uint32_t id : CDids)
      second_border.push_back(id);
    second_border.push_back(Did);

    static_cast<Rectangle*>(base)->mainData->stairs.resize(first_border.size() - 1);
    for (int i = 0; i < first_border.size() - 1; ++i)
      init_stairs(
        first_border[i], first_border[i + 1], second_border[i], second_border[i + 1], 3,
        static_cast<Rectangle*>(base)->mainData->stairs[i]
      );
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Rectangle::UsualFrame::get_points() const
{
  std::set<uint32_t> ids = {
    static_cast<Rectangle*>(base)->mainData->AId,
    static_cast<Rectangle*>(base)->mainData->BId,
    static_cast<Rectangle*>(base)->mainData->CId,
    static_cast<Rectangle*>(base)->mainData->DId,
  };

  for (const auto& id : static_cast<Rectangle*>(base)->mainData->ABIds)
    ids.insert(id);
  for (const auto& id : static_cast<Rectangle*>(base)->mainData->CDIds)
    ids.insert(id);
  for (int i = 0; i < static_cast<Rectangle*>(base)->mainData->stairs.size(); ++i)
    for (const auto& stair : static_cast<Rectangle*>(base)->mainData->stairs[i])
    {
      ids.insert(stair.first);
      ids.insert(stair.second);
    }

  std::vector<Eigen::Vector3d> points;
  for (uint32_t id : ids)
    points.push_back(Points::get_instance().get_point(id));

  return points;
}

void Hexoworld::Rectangle::UsualFrame::print_in_triList(std::vector<uint32_t>& TriList) const {
  for (int j = 0; j < static_cast<Rectangle*>(base)->mainData->stairs.size(); ++j)
    for (int i = 0; i < int(static_cast<Rectangle*>(base)->mainData->stairs[j].size()) - 1; ++i)
      printRect(
        static_cast<Rectangle*>(base)->mainData->stairs[j][i], 
        static_cast<Rectangle*>(base)->mainData->stairs[j][i + 1], 
        TriList
      );
}



Hexoworld::Rectangle::RiverFrame::RiverFrame(Object* base)
  : RectangleFrame(base)
{
  delete_middle_terraces();

  static_cast<Rectangle*>(base)->mainData->stairs[1].clear();
  static_cast<Rectangle*>(base)->mainData->stairs[2].clear();

  waterPointsId.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[1]) +
      base->world.dop_height(River),
      this
    )
  );

  waterPointsId.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[1]) +
      base->world.dop_height(River),
      this
    )
  );

  shorePointsId = {
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[0]) +
        base->world.dop_height(River), 
        this
      ),
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[0]) +
        base->world.dop_height(River), 
        this
      ),
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[2]) +
        base->world.dop_height(River),
        this
      ),
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[2]) +
        base->world.dop_height(River),
        this
      )
  };
}

std::vector<Eigen::Vector3d> Hexoworld::Rectangle::RiverFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& p : waterPointsId)
    answer.push_back(Points::get_instance().get_point(p));
  for (const auto& p : shorePointsId)
    answer.push_back(Points::get_instance().get_point(p));
  return answer;
}

void Hexoworld::Rectangle::RiverFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  printRect(
    { shorePointsId[0],  waterPointsId[0]},
    { shorePointsId[1], waterPointsId[1]},
    TriList
  );
  printRect(
    { shorePointsId[2],  waterPointsId[0]},
    { shorePointsId[3], waterPointsId[1]},
    TriList
  );
}



Hexoworld::Rectangle::RoadFrame::RoadFrame(Object* base)
  : RectangleFrame(base)
{
  delete_middle_terraces();
  static_cast<Rectangle*>(base)->mainData->stairs[1].clear();
  static_cast<Rectangle*>(base)->mainData->stairs[2].clear();

  middlePointsId.push_back(
      Points::get_instance().get_id_point(
        Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[1]) +
        base->world.dop_height(River),
        this
      )
  );
  middlePointsId.push_back(
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[1]) +
      base->world.dop_height(River),
      this
    )
  );

  fencePointsId = {
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[0]) +
      base->world.dop_height(River),
      this
    ),
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[0]) +
      base->world.dop_height(River),
      this
    ),
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->ABIds[2]) +
      base->world.dop_height(River),
      this
    ),
    Points::get_instance().get_id_point(
      Points::get_instance().get_point(static_cast<Rectangle*>(base)->mainData->CDIds[2]) +
      base->world.dop_height(River),
      this
    )
  };
}

std::vector<Eigen::Vector3d> Hexoworld::Rectangle::RoadFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& p : middlePointsId)
    answer.push_back(Points::get_instance().get_point(p));
  for (const auto& p : fencePointsId)
    answer.push_back(Points::get_instance().get_point(p));
  return answer;
}

void Hexoworld::Rectangle::RoadFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  printRect(
    { fencePointsId[0],  middlePointsId[0] },
    { fencePointsId[1], middlePointsId[1] },
    TriList
  );
  printRect(
    { fencePointsId[2],  middlePointsId[0] },
    { fencePointsId[3], middlePointsId[1] },
    TriList
  );
}
