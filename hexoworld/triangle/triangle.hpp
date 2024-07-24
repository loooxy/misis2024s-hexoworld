#pragma once
#include <hexoworld/hexoworld.hpp>

/// \brief Класс треугольник.
class Hexoworld::Triangle : public Object {
public:
  /// \brief Конструктор по умолчанию удалён.
  Triangle() = delete;

  /// \brief Создать треугольник.
  /// \param hexoworld Мир к которому принадлежит треугольник.
  /// \param a Точка a.
  /// \param b Точка b.
  /// \param c Точка с.
  /// \param a_hex Шестиугольник, к которому принадлежит точка a.
  /// \param b_hex Шестиугольник, к которому принадлежит точка b.
  /// \param c_hex Шестиугольник, к которому принадлежит точка c.
  Triangle(Hexoworld& hexoworld,
    Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
    const std::shared_ptr<Hexagon> a_hex,
    const std::shared_ptr<Hexagon> b_hex,
    const std::shared_ptr<Hexagon> c_hex);

  /// \brief Вывести треугольники, на который треангулируется треугольник.
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint16_t>& TriList);

  /// \brief Отрисовщик треугольника.
  class TriangleDrawer : public Drawer<Object> {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит отрисовщик.
    TriangleDrawer(Object* object);
  };

  /// \brief Обычный отрисовщик
  class UsualDrawer : public TriangleDrawer
  {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит отрисовщик.
    /// \param color Основной цвет.
    UsualDrawer(Object* object, Eigen::Vector4i color);

    /// \brief Установить основной цвет.
    /// \param color Цвет.
    void set_color(Eigen::Vector4i color) { color_ = color; }

    /// \brief Получить основной цвет.
    /// \return Цвет.
    Eigen::Vector4i get_color() { return color_; }

    /// \brief Раскрасить точки.
    void colorize_points();
  private:
    Eigen::Vector4i color_; //< Основной цвет.
  };

  /// \brief Основные данные.
  struct MainData {
    uint32_t AId; ///< Id точки a
    uint32_t BId; ///< Id точки b
    uint32_t CId; ///< Id точки c
    std::vector<std::pair<uint32_t, uint32_t>> stairs_down; //< Террасы вниз.
    std::vector<std::pair<uint32_t, uint32_t>> stairs_up; //< Террасы вверх.
    std::vector<uint32_t> middle_triangle; //< Треугольник по середине.
  };

  /// \brief Каркас треугольника.
  class TriangleFrame : public Frame<Object> {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    TriangleFrame(Object* base);
  };

  class UsualFrame : public TriangleFrame {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    /// \param AId Id точки a.
    /// \param BId Id точки b.
    /// \param CId Id точки c.
    UsualFrame(Object* base, uint32_t AId, uint32_t BId, uint32_t CId);

    /// \brief Получить все точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint16_t>& TriList) const;

  private:

    /// \brief Инициализация террас.
    /// \param a Точка a треугольника.
    /// \param b Точка b треугольника.
    /// \param c Точка c треугольника.
    /// \param a_goal Мнимая позиция точки a.
    /// \param b_goal Мнимая позиция точки b.
    /// \param c_goal Мнимая позиция точки c.
    /// \param stairs Куда выводить ступени.
    /// \param cliff Точка от которой не идёт террас.
    /// При 0 террасы идут от всех точек.
    /// При 1 террасы идут только между b и с.
    /// При 2 террасы идут только между a и с.
    /// При 3 террас не будет.
    void init_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
      Eigen::Vector3d a_goal,
      Eigen::Vector3d b_goal,
      Eigen::Vector3d c_goal,
      std::vector<std::pair<uint32_t, uint32_t>>& stairs,
      uint32_t cliff = 0);
  };

private:
  std::shared_ptr<MainData> mainData; //< Основные данные.

  const std::shared_ptr<Hexagon> a_hex; //< Шестиугольник, к которому принадлежит точка a.
  const std::shared_ptr<Hexagon> b_hex; //< Шестиугольник, к которому принадлежит точка b.
  const std::shared_ptr<Hexagon> c_hex; //< Шестиугольник, к которому принадлежит точка c.
};