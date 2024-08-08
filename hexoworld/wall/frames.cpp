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
    std::shared_ptr<MainData> mainData = (
      static_cast<Wall*>(base)->mainData = std::make_shared<MainData>()
      );

    mainData->heightDirection_ = heightDirection.normalized();
    mainData->widthDirection_ = heightDirection.cross(end - start).normalized();

    Eigen::Vector3d v = (end - start).normalized();
    Eigen::Vector3d u = mainData->widthDirection_;

    mainData->bottomRect = {
      {
        Points::get_instance().get_id_point(start - u * width / 2, this),
        Points::get_instance().get_id_point(start + u * width / 2, this)
      },
      {
        Points::get_instance().get_id_point(end - u * width / 2, this),
        Points::get_instance().get_id_point(end + u * width / 2, this)
      }
    };
    mainData->topRect = {
      {
        Points::get_instance().get_id_point(
          start - u * width / 2 + mainData->heightDirection_ * height, this),
        Points::get_instance().get_id_point(
          start + u * width / 2 + mainData->heightDirection_ * height, this)
      },
      {
        Points::get_instance().get_id_point(
          end - u * width / 2 + mainData->heightDirection_ * height, this),
        Points::get_instance().get_id_point(
          end + u * width / 2 + mainData->heightDirection_ * height, this)
      }
    };
  }
}

std::vector<uint32_t> Hexoworld::Wall::UsualFrame::get_pointsId() const
{
  return {
    static_cast<Wall*>(base)->mainData->bottomRect[0].first,
    static_cast<Wall*>(base)->mainData->bottomRect[0].second,
    static_cast<Wall*>(base)->mainData->bottomRect[1].first,
    static_cast<Wall*>(base)->mainData->bottomRect[1].second,
    static_cast<Wall*>(base)->mainData->topRect[0].first,
    static_cast<Wall*>(base)->mainData->topRect[0].second,
    static_cast<Wall*>(base)->mainData->topRect[1].first,
    static_cast<Wall*>(base)->mainData->topRect[1].second,
  };
}
std::vector<Eigen::Vector3d> Hexoworld::Wall::UsualFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (uint32_t i : get_pointsId())
    answer.push_back(Points::get_instance().get_point(i));
  return answer;
}

void Hexoworld::Wall::UsualFrame::print_in_triList(std::vector<uint32_t>& TriList) const {
  const std::shared_ptr<MainData> mainData = static_cast<Wall*>(base)->mainData;
  printRect(
    mainData->bottomRect[0],
    mainData->bottomRect[1],
    TriList
  );
  printRect(
    mainData->topRect[0],
    mainData->topRect[1],
    TriList
  );

  printRect(
    mainData->bottomRect[0],
    mainData->topRect[0],
    TriList
  );
  printRect(
    mainData->bottomRect[1],
    mainData->topRect[1],
    TriList
  );
  printRect(
    {mainData->bottomRect[0].first, mainData->bottomRect[1].first},
    {mainData->topRect[0].first, mainData->topRect[1].first},
    TriList
  );
  printRect(
    {mainData->bottomRect[0].second, mainData->bottomRect[1].second},
    {mainData->topRect[0].second, mainData->topRect[1].second},
    TriList
  );
}