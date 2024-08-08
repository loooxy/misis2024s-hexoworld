#pragma once
#include <hexoworld/hexoworld.hpp>

/// \brief Здание.
class Hexoworld::Cottage : public FixedInventory {
public:
  /// \brief Конструктор по умолчанию удалён.
  Cottage() = delete;

  /// \brief Конструктор.
  /// \param base Объект, к которому относится здание.
  /// \param center Центр здания.
  /// \param mainDirection основное направление.
  /// \param type Тип здания.
  Cottage(Object* base, Eigen::Vector3d center, Eigen::Vector3d mainDirection);

  /// \brief Получить все точки каркаса.
  /// \return Массив точек.
  std::vector<Eigen::Vector3d> get_points() const;

  /// \brief Вывести треугольники.
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint32_t>& TriList) const;

  /// \brief Раскрасить точки.
  void colorize_points();
private:

  /// \brief Отрисовщик коттеджа.
  class CottageDrawer : public Drawer<FixedInventory> {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится отрисовщик.
    CottageDrawer(FixedInventory* base) : Drawer(base) {}

    /// \brief Установить цвет.
    /// \param color Новый цвет.
    void set_color(Eigen::Vector4i color) { color_ = color; }

    /// \brief Получить цвет.
    /// \return Цвет.
    Eigen::Vector4i get_color() { return color_; }

  protected:
    Eigen::Vector4i color_; //< Цвет.
  };

  /// \brief Часть коттеджа.
  enum BuildingParts
  {
    Walls, //< Стены.
    Roof   //< Крыша.
  };

  /// \brief Отрисовщик стен.
  class WallsDrawer : public CottageDrawer {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится отрисовщик.
    WallsDrawer(FixedInventory* base);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Отрисовщик крыши.
  class RoofDrawer : public CottageDrawer {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится отрисовщик.
    RoofDrawer(FixedInventory* base);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Каркас коттеджа
  class CottageFrame : public Frame<FixedInventory> {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится каркас.
    CottageFrame(FixedInventory* base) : Frame(base){}
  };

  /// \brief Каркас стен.
  class WallsFrame : public CottageFrame {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится каркас.
    /// \param center Центр коттеджа.
    /// \param mainDirection Ориентация коттеджа.
    WallsFrame(FixedInventory* base, Eigen::Vector3d center, Eigen::Vector3d mainDirection);

    /// \brief Получение Id точек каркаса.
    /// \return Массив Id-шников.
    std::vector<uint32_t> get_pointsId() const;
    /// \brief Получить точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;
    /// \brief Получить точки основания стен.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points_bottomRect() const;
    /// \brief Получить точки верха стен.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points_topRect() const;

    /// \brief Получить высоту стен.
    /// \return Высота.
    double get_height() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;
  private:
    std::vector<uint32_t> bottomRect_; //< Основание стен.
    std::vector<uint32_t> topRect_;    //< Верхи стен.
  };

  /// \brief Каркас крыши.
  class RoofFrame : public CottageFrame {
  public:
    /// \brief Конструктор.
    /// \param base Коттедж, к которому относится каркас.
    /// \param topRect Верхи стен.
    /// \param mainDirection Ориентация коттеджа.
    RoofFrame(FixedInventory* base, std::vector<Eigen::Vector3d> topRect, 
      Eigen::Vector3d mainDirection);

    /// \brief Получение Id точек каркаса.
    /// \return Массив Id-шников.
    std::vector<uint32_t> get_pointsId() const;
    /// \brief Получить точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;
  private:
    std::vector<uint32_t> bottomRect_;//< Основание крыши.
    std::vector<uint32_t> topLine_;   //< Конёк.
  };
};
