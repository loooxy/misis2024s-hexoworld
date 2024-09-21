#include "cottage.hpp"
#include "cottage.hpp"
#include <hexoworld/cottage/cottage.hpp>

Hexoworld::Cottage::WallsFrame::WallsFrame(FixedInventory* base,
  Eigen::Vector3d center, Eigen::Vector3d mainDirection)
  : CottageFrame(base)
{
  Eigen::Vector3d v = mainDirection * 
    base->base->world.size_ / 14 * 2.5;
  Eigen::Vector3d u = base->base->world.heightDirection_.cross(mainDirection) *
    base->base->world.size_ / 14 * 5;
  Eigen::Vector3d h = base->base->world.heightDirection_ * 
    base->base->world.size_ / 14 * 3;

  bottomRect_ = {
    Points::get_instance().get_id_point(center + v + u, this),
    Points::get_instance().get_id_point(center + v - u, this),
    Points::get_instance().get_id_point(center - v - u, this),
    Points::get_instance().get_id_point(center - v + u, this)
  };

  topRect_ = {
    Points::get_instance().get_id_point(center + v + u + h, this),
    Points::get_instance().get_id_point(center + v - u + h, this),
    Points::get_instance().get_id_point(center - v - u + h, this),
    Points::get_instance().get_id_point(center - v + u + h, this)
  };
}

std::vector<Hexoworld::IdType> Hexoworld::Cottage::WallsFrame::get_pointsId() const
{
  std::vector<IdType> answer;
  for (const auto& point : bottomRect_)
    answer.push_back(point);
  for (const auto& point : topRect_)
    answer.push_back(point);
  return answer;
}
std::vector<Eigen::Vector3d> Hexoworld::Cottage::WallsFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& point : bottomRect_)
    answer.push_back(Points::get_instance().get_point(point));
  for (const auto& point : topRect_)
    answer.push_back(Points::get_instance().get_point(point));
  return answer;
}
std::vector<Eigen::Vector3d> Hexoworld::Cottage::WallsFrame::get_points_bottomRect() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& point : bottomRect_)
    answer.push_back(Points::get_instance().get_point(point));
  return answer;
}
std::vector<Eigen::Vector3d> Hexoworld::Cottage::WallsFrame::get_points_topRect() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& point : topRect_)
    answer.push_back(Points::get_instance().get_point(point));
  return answer;
}

double Hexoworld::Cottage::WallsFrame::get_height() const
{
  return (
    Points::get_instance().get_point(topRect_[0]) - 
    Points::get_instance().get_point(bottomRect_[0])
    ).norm();
}

void Hexoworld::Cottage::WallsFrame::print_in_triList(std::vector<uint32_t>& TriList) const
{
  for (int i = 0; i < 4; ++i)
    printRect(
      {bottomRect_[i], bottomRect_[(i + 1) % 4]},
      {topRect_[i], topRect_[(i + 1) % 4]},
      TriList);
}

Hexoworld::Cottage::RoofFrame::RoofFrame(FixedInventory* base, 
  std::vector<Eigen::Vector3d> topRect,
  Eigen::Vector3d mainDirection)
  : CottageFrame(base)
{
  for (auto& p : topRect)
    bottomRect_.push_back(Points::get_instance().get_id_point(p, this));
  topLine_ = {
    Points::get_instance().get_id_point(
      (topRect[0] + topRect[3]) / 2 + 
      base->base->world.heightDirection_ * base->base->world.size_ / 14 * 1.5,
      this
    ),
    Points::get_instance().get_id_point(
      (topRect[1] + topRect[2]) / 2 +
      base->base->world.heightDirection_ * base->base->world.size_ / 14 * 1.5,
      this
    )
  };
}

std::vector<Hexoworld::IdType> Hexoworld::Cottage::RoofFrame::get_pointsId() const {
  std::vector<IdType> answer;
  for (const auto& point : bottomRect_)
    answer.push_back(point);
  for (const auto& point : topLine_)
    answer.push_back(point);
  return answer;
}
std::vector<Eigen::Vector3d> Hexoworld::Cottage::RoofFrame::get_points() const
{
  std::vector<Eigen::Vector3d> answer;
  for (const auto& point : bottomRect_)
    answer.push_back(Points::get_instance().get_point(point));
  for (const auto& point : topLine_)
    answer.push_back(Points::get_instance().get_point(point));
  return answer;
}

void Hexoworld::Cottage::RoofFrame::print_in_triList(std::vector<uint32_t>& TriList) const {
  printRect(
    {bottomRect_[0], bottomRect_[1]},
    {topLine_[0], topLine_[1]},
    TriList);
  
  printRect(
    {bottomRect_[3], bottomRect_[2]},
    {topLine_[0], topLine_[1]},
    TriList);

  printTri(bottomRect_[0], bottomRect_[3], topLine_[0], TriList);
  printTri(bottomRect_[1], bottomRect_[2], topLine_[1], TriList);
}