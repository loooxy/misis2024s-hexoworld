#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>
#include <hexoworld/hexoworld.hpp>

/// \brief Класс менеджер объектов.
class Hexoworld::Manager {
public:
  /// \brief Конструктор по умолчанию запрещён, так как необходимы данные о мире.
  Manager() = delete;

  /// \brief Создание менеджера.
  /// \param hexoworld Мир, к которому принадлежит менеджер.
  Manager(Hexoworld& hexoworld) : world(hexoworld) {}

  /// \brief Получить все объекты.
  /// \return Массив объектов.
  std::vector<std::shared_ptr<Object>> get_all_object();

  /// \brief Получить шестиугольник по координате.
  /// \param coord Координата.
  /// \return Шестиугольник.
  std::shared_ptr<Hexagon> get_hexagon(Coord coord);

  /// \brief Получить прямоугольник, который между двух шестиугольников.
  /// \param first Координата первого шестиугольника.
  /// \param second Координата второго шестиугольника.
  /// \return Прямоугольник.
  std::shared_ptr<Rectangle> get_rectangle(Coord first, Coord second);

  /// \brief Получить треугольник, который между трёх шестиугольников.
  /// \param first Координата первого шестиугольника.
  /// \param second Координата второго шестиугольника.
  /// \param third Координата третьего шестиугольника.
  /// \return Треугольник.
  std::shared_ptr<Triangle> get_triangle(Coord first, Coord second, Coord third);

  /// \brief Получить координаты соседних шестиугольников.
  /// \param pos Позиция шестиугольника.
  /// \return Множество координат соседних шестиугольников.
  std::set<Coord> get_neighbors(Coord pos);

  /// \brief Добавить реку.
  /// \param river Объекты, по которым проходит река. 
  void add_river(std::vector<std::shared_ptr<Object>> river);
  
  /// \brief Получить все реки.
  /// \return Массив массивов объектов, по которым протекает очередная река.
  std::vector<std::vector<std::shared_ptr<Object>>> get_all_rivers() const;
private:
  /// \brief Создать шестиугольник.
  /// \param coord Координата.
  /// return Шестиугольник.
  std::shared_ptr<Hexagon> add_hexagon(Coord coord);

  /// \brief Создать прямоугольник, который между двух шестиугольников.
  /// \param first Координата первого шестиугольника.
  /// \param second Координата второго шестиугольника.
  /// \return Прямоугольник.
  std::shared_ptr<Rectangle> add_rectangle(Coord first, Coord second);

  /// \brief Создать треугольник, который между трёх шестиугольников.
  /// \param first Координата первого шестиугольника.
  /// \param second Координата второго шестиугольника.
  /// \param third Координата третьего шестиугольника.
  /// \return Треугольник.
  std::shared_ptr<Triangle> add_triangle(Coord first, Coord second, Coord third);

  std::map<Coord, std::shared_ptr<Hexagon>> grid_; ///< Все шестиугольники.
  std::map<std::pair<Coord, Coord>, std::shared_ptr<Rectangle>> rectangles; ///< Все межшестиугольные прямоугольники.
  std::map<std::vector<Coord>, std::shared_ptr<Triangle>> triangles; ///< Все межшестиугольные треугольники.
  Hexoworld& world; ///< Мир, к которому принадлежит шестиугольная сетка.
  std::vector<std::vector<std::shared_ptr<Object>>> rivers; //< Реки.
};
