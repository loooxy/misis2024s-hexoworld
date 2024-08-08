#include <hexoworld/hexoworld.hpp>
#include <iostream>
#include <thread>

void Hexoworld::Points::set_point_color(uint32_t point, Eigen::Vector4i color)
{
  id_to_color.at(point / colors_on_point).at(point % colors_on_point) = color;
}

Eigen::Vector4i Hexoworld::Points::get_point_color(Eigen::Vector3d point, const Essence* base)
{
  uint32_t id = get_id_point(point, base);
  return id_to_color.at(id / colors_on_point).at(id % colors_on_point);
}

bool Hexoworld::Points::in_points(Eigen::Vector3d p) const
{
  return (point_to_id.find(p) != point_to_id.end());
}

uint32_t Hexoworld::Points::get_id_point(Eigen::Vector3d p, const Essence* base)
{
  uint32_t id;
  if (!in_points(p))
  {
    id = point_to_id[p] = id_to_point.size();
    id_to_point.push_back(p);
    id_to_color.push_back(std::vector<Eigen::Vector4i>(colors_on_point, Eigen::Vector4i(0, 0, 0, 255)));
    id_to_essences.push_back(std::map<const Essence*, uint32_t>());
  }
  else
  {
    id = point_to_id[p];
  }

  if (id_to_essences[id].find(base) == id_to_essences[id].end())
  {
    uint32_t len = id_to_essences[id].size();
    id_to_essences[id][base] = len;
  }

  return id * colors_on_point + id_to_essences[id][base];
}

Eigen::Vector3d Hexoworld::Points::get_point(uint32_t id) const
{
  if (id / colors_on_point >= id_to_point.size())
    throw std::invalid_argument("wrong id");

  return id_to_point.at(id / colors_on_point);
}

void Hexoworld::Points::update_point(uint32_t id, Eigen::Vector3d new_point)
{
  Eigen::Vector3d point = id_to_point[id / colors_on_point];
  point_to_id.erase(point);
  point_to_id[new_point] = id / colors_on_point;
  id_to_point[id / colors_on_point] = new_point;
}

void Hexoworld::Points::print_in_vertices(std::vector<PrintingPoint>& Vertices)
{
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

