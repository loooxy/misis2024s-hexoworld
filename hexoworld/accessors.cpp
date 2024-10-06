#include <hexoworld/hexoworld.hpp>
#include <iostream>
#include <thread>

Hexoworld::FloodAccessor::FloodAccessor(Hexoworld* world, uint32_t row, uint32_t col)  
  : world_(world), row_(row), col_(col) {}

bool Hexoworld::FloodAccessor::operator=(bool value)
{
  if (value)
    world_->add_flood_in_hex(row_, col_);
  else
    world_->del_flood_in_hex(row_, col_);

  return value;
}

Hexoworld::FloodAccessor::operator bool() const
{
  return world_->main_data_.get_floods(Coord(row_, col_));
}

Hexoworld::FloodAccessor Hexoworld::flood(uint32_t row, uint32_t col)
{
  return FloodAccessor(this, row, col);
}

Hexoworld::RoadAccessor::RoadAccessor(Hexoworld* world, uint32_t row, uint32_t col)
  : world_(world), row_(row), col_(col) {}

bool Hexoworld::RoadAccessor::operator=(bool value)
{
  if (value)
    world_->add_road_in_hex(row_, col_);
  else
    world_->del_road_in_hex(row_, col_);

  world_->main_data_.set_roads(Coord(row_, col_), value);

  return value;
}

Hexoworld::RoadAccessor::operator bool() const
{
  return world_->main_data_.get_roads(Coord(row_, col_));
}

Hexoworld::RoadAccessor Hexoworld::road(uint32_t row, uint32_t col)
{
  return RoadAccessor(this, row, col);
}

Hexoworld::FarmAccessor::FarmAccessor(Hexoworld* world, uint32_t row, uint32_t col)
  : world_(world), row_(row), col_(col) {}

bool Hexoworld::FarmAccessor::operator=(bool value)
{
  if (value)
    world_->add_farm_in_hex(row_, col_);
  else
    world_->del_farm_in_hex(row_, col_);

  world_->main_data_.set_farms(Coord(row_, col_), value);

  return value;
}

Hexoworld::FarmAccessor::operator bool() const
{
  return world_->main_data_.get_farms(Coord(row_, col_));
}

Hexoworld::FarmAccessor Hexoworld::farm(uint32_t row, uint32_t col)
{
  return FarmAccessor(this, row, col);
}


Hexoworld::HeightAccessor::HeightAccessor(Hexoworld* world, uint32_t row, uint32_t col)
  : world_(world), row_(row), col_(col) {}

int32_t Hexoworld::HeightAccessor::operator=(int32_t value)
{
  world_->set_hex_height(row_, col_, value);

  world_->main_data_.set_heights(Coord(row_, col_), value);

  return value;
}

Hexoworld::HeightAccessor::operator int32_t() const
{
  return world_->main_data_.get_heights(Coord(row_, col_));
}

Hexoworld::HeightAccessor Hexoworld::height(uint32_t row, uint32_t col)
{
  return HeightAccessor(this, row, col);
}


Hexoworld::ColorAccessor::ColorAccessor(Hexoworld* world, uint32_t row, uint32_t col)
  : world_(world), row_(row), col_(col) {}

Eigen::Vector4i Hexoworld::ColorAccessor::operator=(Eigen::Vector4i value)
{
  world_->set_hex_color(row_, col_, value);

  world_->main_data_.set_colors(Coord(row_, col_), value);

  return value;
}

Hexoworld::ColorAccessor::operator Eigen::Vector4i() const
{
  return world_->main_data_.get_colors(Coord(row_, col_));
}

Hexoworld::ColorAccessor Hexoworld::color(uint32_t row, uint32_t col)
{
  return ColorAccessor(this, row, col);
}
