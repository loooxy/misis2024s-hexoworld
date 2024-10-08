#pragma once
#include <hexoworld/hexoworld.hpp>

/// \brief Стена.
struct Hexoworld::Wall : public FixedInventory {
  /// \brief Конструктор по умолчанию удалён.
  Wall() = delete;

  /// \brief Конструктор.
  /// \param base Объект, к которому принадлежит стена.
  /// \param start Начало стены.
  /// \param end Конец стены.
  /// \param height Высота стены.
  /// \param width Ширина стены.
  /// \param color Цвет стены.
  Wall(Object* base, Eigen::Vector3d start, Eigen::Vector3d end, 
    double height, double width, Eigen::Vector4i color);

  /// \brief Конструктор.
  /// \param base Объект, к которому принадлежит стена.
  /// \param start Начало стены.
  /// \param end Конец стены.
  /// \param height Высота стены.
  /// \param width Ширина стены.
  /// \param heightDirection Направление высоты.
  /// \param widthDirection Направление ширины.
  /// \param color Цвет стены.
  Wall(Object* base, Eigen::Vector3d start, Eigen::Vector3d end,
    double height, double width,
    Eigen::Vector3d heightDirection,
    Eigen::Vector4i color);

  /// \brief Получить точки стены.
  /// \return Массив точек.
  std::vector<Eigen::Vector3d> get_points() const;

  /// \brief Вывести треугольники, на который треангулируется треугольник.
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint32_t>& TriList) const;
  void colorize_points();

  /// \brief Отрисовщик стены.
  class WallDrawer : public Drawer<FixedInventory> {
  public:
    /// \brief Конструктор
    /// \param object Стена, к которой относится отрисовщик.
    WallDrawer(FixedInventory* object);
  };

  /// \brief Обычный обязательный отрисовщик.
  class UsualDrawer : public WallDrawer {
  public:
    /// \brief Конструктор.
    /// \param base Стена, к которой относится отрисовщик.
    /// \param color Цвет стены.
    UsualDrawer(FixedInventory* base, Eigen::Vector4i color);

    /// \brief Установить основной цвет стены.
    /// \param color Цвет.
    void set_color(Eigen::Vector4i color) { color_ = color; }

    /// \brief Получить основной цвет стены.
    /// \return Цвет.
    Eigen::Vector4i get_color() const { return color_; }

    /// \brief Раскрасить точки.
    void colorize_points();
  private:
    Eigen::Vector4i color_; //< Основной цвет стены. 
  };

  /// \brief Основные данные о каркасе.
  struct MainData {
    std::vector<std::pair<uint32_t, uint32_t>> bottomRect; //< Прямоугольник основания.
    std::vector<std::pair<uint32_t, uint32_t>> topRect;    //< Прямоугольник верха.
    Eigen::Vector3d heightDirection_; //< Направление вверх.
    Eigen::Vector3d widthDirection_; //< Направление вширь.
  };

  /// \brief Каркас шестиугольника.
  struct WallFrame : public Frame<FixedInventory>{
    /// \brief Конструктор.
    /// \param base Стена, к которой относится каркас.
    WallFrame(FixedInventory* base);
  };

  /// \brief Обычный каркас.
  struct UsualFrame : public WallFrame {
    /// \brief Создает обычный каркас стены.
    /// \param base Стена, к которой относится каркас.
    /// \param start Начало стены.
    /// \param end Конец стены.
    /// \param height Высота стены.
    /// \param width Ширина стены.
    /// \param heightDirection Направление высоты.
    UsualFrame(FixedInventory* base, 
      Eigen::Vector3d start, Eigen::Vector3d end, 
      double height, double width,
      Eigen::Vector3d heightDirection);

    /// \brief Получение Id точек каркаса.
    /// \return Массив Id-шников.
    std::vector<uint32_t> get_pointsId() const;

    /// \brief Получить точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;
  };

  std::shared_ptr<MainData> mainData; //< Основные данные о каркасе.
};
