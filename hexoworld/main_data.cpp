#include <hexoworld/hexoworld.hpp>
#include <iostream>
#include <thread>

void Hexoworld::MainData::set_heights(Coord pos, int32_t height) {
  mtx.lock();
  heights[pos] = height;
  mtx.unlock();
}

int32_t Hexoworld::MainData::get_heights(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);
  
  if (heights.find(pos) == heights.end())
    return 0;

  return heights.at(pos);
}

void Hexoworld::MainData::set_colors(Coord pos, Eigen::Vector4i color)
{
  mtx.lock();
  colors[pos] = color;
  mtx.unlock();
}

Eigen::Vector4i Hexoworld::MainData::get_colors(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);

  if (colors.find(pos) == colors.end())
    return Eigen::Vector4i();

  return colors.at(pos);
}

void Hexoworld::MainData::set_rivers(Coord pos, bool river)
{
  mtx.lock();
  rivers[pos] = river;
  mtx.unlock();
}

bool Hexoworld::MainData::get_rivers(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);

  if (rivers.find(pos) == rivers.end())
    return false;

  return rivers.at(pos);
}

void Hexoworld::MainData::set_roads(Coord pos, bool road)
{
  mtx.lock();
  roads[pos] = road;
  mtx.unlock();
}

bool Hexoworld::MainData::get_roads(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);

  if (roads.find(pos) == roads.end())
    return false;

  return roads.at(pos);
}

void Hexoworld::MainData::set_farms(Coord pos, bool farm)
{
  mtx.lock();
  farms[pos] = farm;
  mtx.unlock();
}

bool Hexoworld::MainData::get_farms(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);

  if (farms.find(pos) == farms.end())
    return false;

  return farms.at(pos);
}

void Hexoworld::MainData::set_floods(Coord pos, bool flood)
{
  mtx.lock();
  floods[pos] = flood;
  mtx.unlock();
}

bool Hexoworld::MainData::get_floods(Coord pos) const
{
  std::lock_guard<std::mutex> locker(mtx);

  if (floods.find(pos) == floods.end())
    return false;

  return floods.at(pos);
}
