#include <hexoworld/hexoworld.hpp>

Hexoworld::Wall::WallFrame::WallFrame(FixedInventory* base)
  : Frame(base)
{}

Hexoworld::Wall::UsualFrame::UsualFrame(FixedInventory* base,
  Eigen::Vector3d start, Eigen::Vector3d end, 
  double height, double width)
  : WallFrame(base)
{
  if (static_cast<Wall*>(base)->mainData == nullptr)
  {
    std::shared_ptr<MainData> mainData = (
      static_cast<Wall*>(base)->mainData = std::make_shared<MainData>()
      );
    Eigen::Vector3d v = (end - start).normalized();
    Eigen::Vector3d u = v.cross(base->base->world.heightDirection_).normalized();

    mainData->bottomRect = {
      {
        start - u * width / 2,
        start + u * width / 2
      },
      {
        end - u * width / 2,
        end + u * width / 2
      }
    };
    mainData->topRect = {
      {
        start - u * width / 2 + base->base->world.heightDirection_ * height,
        start + u * width / 2 + base->base->world.heightDirection_ * height
      },
      {
        end - u * width / 2 + base->base->world.heightDirection_ * height,
        end + u * width / 2 + base->base->world.heightDirection_ * height
      }
    };
  }
}

std::vector<Eigen::Vector3d> Hexoworld::Wall::UsualFrame::get_points() const
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

void Hexoworld::Wall::UsualFrame::print_in_triList(std::vector<uint16_t>& TriList) const {
  const std::shared_ptr<MainData> mainData = static_cast<Wall*>(base)->mainData;
  printRect(
    mainData->bottomRect[0],
    mainData->bottomRect[1],
    TriList, base->base
  );
  printRect(
    mainData->topRect[0],
    mainData->topRect[1],
    TriList, base->base
  );

  printRect(
    mainData->bottomRect[0],
    mainData->topRect[0],
    TriList, base->base
  );
  printRect(
    mainData->bottomRect[1],
    mainData->topRect[1],
    TriList, base->base
  );
  printRect(
    {mainData->bottomRect[0].first, mainData->bottomRect[1].first},
    {mainData->topRect[0].first, mainData->topRect[1].first},
    TriList, base->base
  );
  printRect(
    {mainData->bottomRect[0].second, mainData->bottomRect[1].second},
    {mainData->topRect[0].second, mainData->topRect[1].second},
    TriList, base->base
  );
}