#include <hexoworld/hexoworld.hpp>
#include <iostream>

void Hexoworld::Points::set_point_color(Eigen::Vector3d point, Eigen::Vector4i color, const Object* base)
{
  uint32_t id = get_id_point(point, base);
  id_to_color[id / colors_on_point][id % colors_on_point] = color;
}

bool Hexoworld::Points::in_points(Eigen::Vector3d p) const
{
  return (point_to_id.find(p) != point_to_id.end());
}

uint32_t Hexoworld::Points::get_id_point(Eigen::Vector3d p, const Object* base)
{
  uint32_t id;
  if (!in_points(p))
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_color.push_back(std::vector<Eigen::Vector4i>(colors_on_point, Eigen::Vector4i(0, 0, 0, 255)));
    id_to_objects.push_back(std::map<const Object*, uint32_t>());
  }
  else
  {
    id = point_to_id[p];
  }

  if (id_to_objects[id].find(base) == id_to_objects[id].end())
  {
    uint32_t len = id_to_objects[id].size();
    id_to_objects[id][base] = len;
  }

  return id * colors_on_point + id_to_objects[id][base];
}

Eigen::Vector3d Hexoworld::Points::get_point(uint32_t id) const
{
  if (id / colors_on_point >= id_to_point.size())
    throw std::invalid_argument("wrong id");
  return id_to_point[id / colors_on_point];
}

void Hexoworld::Points::update_point(uint32_t id, Eigen::Vector3d new_point)
{
  Eigen::Vector3d point = Points::get_instance().id_to_point[id / colors_on_point];
  Points::get_instance().point_to_id.erase(point);
  Points::get_instance().point_to_id[new_point] = id / colors_on_point;
  Points::get_instance().id_to_point[id / colors_on_point] = new_point;
}