#include "hexoworld.hpp"
#include <hexoworld/hexoworld.hpp>
#include <iostream>
#include <thread>

void Hexoworld::Points::set_point_color(const IdType& point, Eigen::Vector4i color)
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());
  uint32_t id = point.get();
  id_to_color.at(id / colors_on_point).at(id % colors_on_point) = color;
}

Eigen::Vector4i Hexoworld::Points::get_point_color(Eigen::Vector3d point, const Essence* base)
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());
  uint32_t id = get_id_point(point, base).get();
  return id_to_color.at(id / colors_on_point).at(id % colors_on_point);
}

bool Hexoworld::Points::in_points(Eigen::Vector3d p) const
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());
  return (point_to_id.find(p) != point_to_id.end());
}

Hexoworld::IdType Hexoworld::Points::get_id_point(Eigen::Vector3d p, const Essence* base)
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());
  uint32_t short_id;
  if (!in_points(p))
  {
    if (is_locked)
      throw std::runtime_error("Points was locked");

    short_id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_color.push_back(std::vector<Eigen::Vector4i>(colors_on_point, Eigen::Vector4i(0, 0, 0, 255)));
    id_to_essences.push_back(std::map<const Essence*, uint32_t>());
    points_id.push_back(std::map<uint32_t, const Essence*>());
  }
  else
  {
    short_id = point_to_id[p];
  }

  if (id_to_essences[short_id].find(base) == id_to_essences[short_id].end())
  {
    if (is_locked)
      throw std::runtime_error("Points was locked");

    for (uint32_t i = 0; i < colors_on_point; ++i)
      if (points_id[short_id].find(i) == points_id[short_id].end())
      {
        id_to_essences[short_id][base] = i;
        points_id[short_id][i] = base;
        break;
      }
    
    if (id_to_essences[short_id][base] == uint32_t(-1))
    {
      std::cout << p << std::endl;
      throw std::runtime_error("Не хватает количества точек");
    }
  }

  return IdType(short_id * colors_on_point + id_to_essences.at(short_id).at(base));
}

Eigen::Vector3d Hexoworld::Points::get_point(const IdType& id) const
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());

  uint32_t id_ = id.get();
  if (id_ / colors_on_point >= id_to_point.size())
    throw std::invalid_argument("wrong id");

  return id_to_point.at(id_ / colors_on_point);
}

void Hexoworld::Points::update_point(const IdType& id, Eigen::Vector3d new_point)
{
  if (is_locked)
    throw std::runtime_error("Points was locked");

  OneThreadController __otc__(0, std::this_thread::get_id());

  uint32_t id_ = id.get();

  Eigen::Vector3d point = id_to_point[id_ / colors_on_point];
  point_to_id.erase(point);
  point_to_id[new_point] = id_ / colors_on_point;
  id_to_point[id_ / colors_on_point] = new_point;
}

void Hexoworld::Points::print_in_vertices(std::vector<PrintingPoint>& Vertices)
{
  OneThreadController __otc__(0, std::this_thread::get_id());
#ifdef PARALLEL

  Vertices.resize(id_to_point.size() * colors_on_point);
  uint32_t n_threads = Vertices.size() / 30000 + 1;
  
  auto func = [this, n_threads, &Vertices](uint32_t phase) -> void {
    for (int i = phase; i < id_to_point.size(); i += n_threads)
    {
      for (int j = 0; j < colors_on_point; ++j)
        Vertices[i * colors_on_point + j] = PrintingPoint(id_to_point[i], id_to_color[i][j]);
    }
    };

  std::vector<std::thread> threads;
  for (int i = 0; i < n_threads; ++i)
    threads.push_back(std::thread(func, i));

  for (int i = 0; i < n_threads; ++i)
    threads[i].join();

#else

  Vertices.resize(id_to_point.size() * colors_on_point);
  for (int i = 0; i < id_to_point.size(); ++i)
  {
    for (int j = 0; j < colors_on_point; ++j)
      Vertices[i * colors_on_point + j] = PrintingPoint(id_to_point[i], id_to_color[i][j]);
  }

#endif // PARALLEL
}

