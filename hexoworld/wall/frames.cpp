#include "wall.hpp"
#include <hexoworld/wall/wall.hpp>

Hexoworld::Wall::WallFrame::WallFrame(FixedInventory* base)
  : Frame(base)
{}

Hexoworld::Wall::UsualFrame::UsualFrame(FixedInventory* base,
  Eigen::Vector3d start, Eigen::Vector3d end, 
  double height, double width, 
  Eigen::Vector3d heightDirection)
  : WallFrame(base)
{
  if (static_cast<Wall*>(base)->mainData == nullptr)
  {
    heightDirection.normalize();

    Eigen::Vector3d widthDirection = heightDirection.cross(end - start).normalized();

    IdType a_bottom = Points::get_instance().get_id_point(start - widthDirection * width / 2, this);
    IdType b_bottom = Points::get_instance().get_id_point(start + widthDirection * width / 2, this);
    IdType c_bottom = Points::get_instance().get_id_point(end - widthDirection * width / 2, this);
    IdType d_bottom = Points::get_instance().get_id_point(end + widthDirection * width / 2, this);

    IdType a_top = Points::get_instance().get_id_point(
          start - widthDirection * width / 2 + heightDirection * height, this);
    IdType b_top = Points::get_instance().get_id_point(
          start + widthDirection * width / 2 + heightDirection * height, this);
    IdType c_top = Points::get_instance().get_id_point(
          end - widthDirection * width / 2 + heightDirection * height, this);
    IdType d_top = Points::get_instance().get_id_point(
          end + widthDirection * width / 2 + heightDirection * height, this);

    std::shared_ptr<MainData> mainData = (
      static_cast<Wall*>(base)->mainData = std::make_shared<MainData>(
        a_bottom,
        b_bottom,
        c_bottom,
        d_bottom,
          
        a_top,
        b_top,
        c_top,
        d_top,
          
        heightDirection,
        widthDirection
      )
      );
  }
}

std::vector<Hexoworld::IdType> Hexoworld::Wall::UsualFrame::get_pointsId() const
{
  return {
    static_cast<Wall*>(base)->mainData->bottomRect_[0].first,
    static_cast<Wall*>(base)->mainData->bottomRect_[0].second,
    static_cast<Wall*>(base)->mainData->bottomRect_[1].first,
    static_cast<Wall*>(base)->mainData->bottomRect_[1].second,
    static_cast<Wall*>(base)->mainData->topRect_[0].first,
    static_cast<Wall*>(base)->mainData->topRect_[0].second,
    static_cast<Wall*>(base)->mainData->topRect_[1].first,
    static_cast<Wall*>(base)->mainData->topRect_[1].second,
  };
}
std::vector<Eigen::Vector3d> Hexoworld::Wall::UsualFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (IdType i : get_pointsId())
    answer.push_back(Points::get_instance().get_point(i));
  return answer;
}

void Hexoworld::Wall::UsualFrame::print_in_triList(std::vector<uint32_t>& TriList) const {
  const std::shared_ptr<MainData> mainData = static_cast<Wall*>(base)->mainData;
  printRect(
    mainData->bottomRect_[0],
    mainData->bottomRect_[1],
    TriList
  );
  printRect(
    mainData->topRect_[0],
    mainData->topRect_[1],
    TriList
  );

  printRect(
    mainData->bottomRect_[0],
    mainData->topRect_[0],
    TriList
  );
  printRect(
    mainData->bottomRect_[1],
    mainData->topRect_[1],
    TriList
  );
  printRect(
    {mainData->bottomRect_[0].first, mainData->bottomRect_[1].first},
    {mainData->topRect_[0].first, mainData->topRect_[1].first},
    TriList
  );
  printRect(
    {mainData->bottomRect_[0].second, mainData->bottomRect_[1].second},
    {mainData->topRect_[0].second, mainData->topRect_[1].second},
    TriList
  );
}