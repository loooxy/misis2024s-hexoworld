#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>
#include <hexoworld/hexoworld.hpp>

class Hexoworld::TextureGrid {
public:
  /// \brief Конструктор по умолчанию.
  TextureGrid() = default;

  /// \brief Установить цвет точки.
  /// \param pos Позиция точки.
  /// \param color Устанавливаемый цвет.
  /// \param n_parts Число частей цвета (нужно будет для смешивания).
  void set_point(Eigen::Vector3d pos, Eigen::Vector4i color, uint32_t n_parts = 1);

  /// \brief Раскрасить вершины. 
  /// \param Vertices Вершины.
  void colorize_vertices(std::vector<PrintingPoint>& Vertices) const;
private:
  /// \brief Класс Color нужен для хранения цвета.
  class Color {
  public:
    /// \brief Создаёт чёрный непрозрачный цвет.
    Color();

    /// \brief Создаёт цвет.
    /// \param red Красная компонента цвета
    /// \param blue Синяя компонента цвета
    /// \param green Зелёная компонента цвета
    /// \param alpha Компонента прозрачности цвета
    /// \param n_parts Число частей цвета (нужно будет для смешивания).
    Color(uint8_t red, uint8_t blue,
      uint8_t green, uint8_t alpha = 255, uint32_t n_parts = 1);

    /// \brief Создаёт цвет.
    /// \param abgr Цвет в формате alpha-blue-green-red.
    /// \param n_parts Число частей цвета (нужно будет для смешивания).
    Color(uint32_t abgr, uint32_t n_parts = 1);

    /// \brief Операция + смешивает два цвета в пропорциях n_parts.
    /// \param rhs Цвет, с которым смешиваем.
    /// \return Новый цвет.
    Color operator+ (const Color& rhs) const;

    /// \brief get_abgr() возвращает цвет в формате alpha-blue-green-red.
    /// \return Цвет в формате alpha-blue-green-red.
    uint32_t get_abgr() const;
  private:
    uint32_t abgr_; ///< Цвет в формате alpha-blue-green-red.
    uint32_t n_parts_; ///< Число частей цвета (нужно будет для смешивания).
  };

  /// \brief Получить цвет точки, по её позиции.
  /// \param position Позиция точки.
  /// \return Цвет точки.
  Color get_point_color(Eigen::Vector3d position) const;

  std::map<Eigen::Vector3d, Color, EigenVector3dComp> points; ///< Цвета точек.
};