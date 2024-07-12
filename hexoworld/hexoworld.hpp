#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>

/// \brief Структура формата вывода точки.
struct PrintingPoint {
  /// \brief Конструктор по умолчанию
  PrintingPoint() = default;
  /// \brief Конструктор по позиции.
  /// \param position Позиция точки.
  PrintingPoint(Eigen::Vector3d position) 
    : x(position.x()), y(position.y()), z(position.z()) {};
  float x, y, z;
  uint32_t abgr;
};

/// \brief Класс шестиугольного мира.
class Hexoworld
{
public:
  /// \brief Конструктор по умолчанию запрещён, так как необходимы параметры мира.
  Hexoworld() = delete;

  /// \brief Создает шестиугольный мир.
  /// \param size Радиус шестиугольников покрытия.
  /// \param origin Начало координат.
  /// \param row_direction Направление увеличения номера строки.
  /// \param сol_direction Направление увеличения номера столбца.
  /// !!! должно быть перпендикулярно row_direction !!!
  /// \param n_rows Количество строк.
  /// \param n_cols Количество столбцов.
  /// \param height_step Высота между соседними уровнями.
  /// \param n_terraces_on_height_step Количество террас между соседними уровнями.
  Hexoworld(float size, Eigen::Vector3d origin,
    Eigen::Vector3d row_direction, Eigen::Vector3d сol_direction,
    uint32_t n_rows = 0, uint32_t n_cols = 0,
    float height_step = 1.0f,
    uint32_t n_terraces_on_height_step = 2);

  /// \brief Добавить шестиугольник.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param color Цвет шестиугольника.
  void add_hexagon(uint32_t row, uint32_t col, Eigen::Vector4i color);

  /// \brief Добавить реку
  /// \param hexs Координаты {row, col} шестиугольников, по которым течёт река.
  void add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs);

  /// \brief Установить высоту шестиугольнику.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param height Новая высота.
  void set_hex_height(uint32_t row, uint32_t col, int32_t height);

  /// \brief Установить цвет шестиугольнику.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param color Новый цвет шестиугольника.
  void set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color);

  /// \brief Вывести вершины и треугольники, на который треангулируется мир.
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;
private:
  /// \brief Класс-компаратор для Eigen::Vector3d
  class EigenVector3dComp {
  public:
    EigenVector3dComp() = default;
    bool operator()(const Eigen::Vector3d& a,
      const Eigen::Vector3d& b) const {
      if (a.x() + PRECISION_DBL_CALC < b.x())
        return true;
      if (b.x() + PRECISION_DBL_CALC < a.x())
        return false;

      if (a.y() + PRECISION_DBL_CALC < b.y())
        return true;
      if (b.y() + PRECISION_DBL_CALC < a.y())
        return false;

      if (a.z() + PRECISION_DBL_CALC < b.z())
        return true;

      return false;
    }
  };

  /// \brief Класс текстур
  class TextureGrid;

  /// \brief Класс шестиугольной сетки.
  class HexagonGrid;

  std::unique_ptr<HexagonGrid> hexagonGrid_; ///< Шестиугольная сетка.
  std::unique_ptr<TextureGrid> textureGrid_; ///< Текстурная сетка.
  static const double PRECISION_DBL_CALC; ///< Точность вещественных вычислений.
};