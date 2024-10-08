#pragma once
#include <hexoworld/hexoworld.hpp>

/// \brief Класс прямоугольник между двух шестиугольников 
class Hexoworld::Rectangle : public Object {
public:
  /// \brief Конструктор по умолчанию удалён
  Rectangle() = delete;

  /// \brief Создание прямоугольника.
  /// \param hexoworld Мир к которому принадлежит прямоугольник.
  /// \param a Точка a.
  /// \param b Точка b.
  /// \param c Точка c.
  /// \param d Точка d.
  /// \param ab_points Дополнительные точки между a и b.
  /// \param cd_points Дополнительные точки между c и d.
  /// \param ab_hex Шестиугольник, содержащий точки a и b.
  /// \param cd_hex Шестиугольник, содержащий точки c и d.
  Rectangle(Hexoworld& hexoworld,
    Eigen::Vector3d a, Eigen::Vector3d b,
    Eigen::Vector3d c, Eigen::Vector3d d,
    std::vector<Eigen::Vector3d> ab_points,
    std::vector<Eigen::Vector3d> cd_points,
    const std::shared_ptr<Hexagon> ab_hex, const std::shared_ptr<Hexagon> cd_hex);

  /// \brief Обновление прямоугольника.
  void update();

  /// \brief Создать реку в этом прямоугольнике.
  void make_river();

  /// \brief Создать дорогу в этом прямоугольнике
  void make_road();

  /// \brief Вывести треугольники, на который треангулируется прямоугольник.
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint32_t>& TriList);

  /// \brief Раскрасить точки.
  void colorize_points();

  /// \brief Отрисовщик прямоугольника.
  class RectangleDrawer : public Drawer<Object> {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    RectangleDrawer(Object* object);
  };

  /// \brief Обычный обязательный отрисовщик.
  class UsualDrawer : public RectangleDrawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    /// \param color Основной цвет.
    UsualDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Отрисовщик рек.
  class RiverDrawer : public RectangleDrawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит отрисовщик.
    RiverDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();

    /// \brief Установить специальный цвет реки.
    /// \param new_color Цвет. 
    void set_new_special_color_river(Eigen::Vector4i new_color);

    Eigen::Vector4i special_color_river; //< Специальный цвет реки.
  };

  /// \brief Отрисовщик дорог.
  class RoadDrawer : public RectangleDrawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит отрисовщик.
    RoadDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Основные данные.
  struct MainData {
    uint32_t AId; ///< Id точки a
    uint32_t BId; ///< Id точки b
    uint32_t CId; ///< Id точки c
    uint32_t DId; ///< Id точки d
    std::vector<uint32_t> ABIds, CDIds; //< Точки между гранями.
    std::vector<std::vector<std::pair<uint32_t, uint32_t>>> stairs; //< террасы.
  };

  /// \brief Каркас прямоугольника.
  class RectangleFrame : public Frame<Object> {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    RectangleFrame(Object* base);
  protected:
    /// \brief Инициализация террас.
    /// \param AId Точка a.
    /// \param BId Точка b.
    /// \param CId Точка c.
    /// \param DId Точка d.
    /// \param terraces_side Сторона, на которой выводить террасы.
    /// \param stairs Куда выводить террасы.
    void init_stairs(uint32_t AId, uint32_t BId, uint32_t CId, uint32_t DId,
      uint32_t terraces_side, std::vector<std::pair<uint32_t, uint32_t>>& stairs);

    /// \brief Удаляет террасы по середине.
    void delete_middle_terraces();
  };

  /// \brief Обычный каркас
  class UsualFrame : public RectangleFrame {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    /// \param Aid Точка a.
    /// \param Bid Точка b.
    /// \param Cid Точка c.
    /// \param Did Точка d.
    /// \param ABids Точки, между точками a и b.
    /// \param CDids Точки, между точками c и d.
    UsualFrame(Object* base, uint32_t Aid, uint32_t Bid, uint32_t Cid, uint32_t Did,
      std::vector<uint32_t> ABids, std::vector<uint32_t> CDids);

    /// \brief Получить все точки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;
  };

  /// \brief Речной каркас.
  class RiverFrame : public RectangleFrame {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    RiverFrame(Object* base);

    /// \brief Получить все точки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;

    std::vector<uint32_t> waterPointsId; //< Точки, принадлежащие воде.
    std::vector<uint32_t> shorePointsId; //< Точки берега.
  };

  /// \brief Каркас дороги.
  class RoadFrame : public RectangleFrame {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    RoadFrame(Object* base);

    /// \brief Получить все точки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;

    std::vector<uint32_t> middlePointsId; //< Точки посередине дороги.
    std::vector<uint32_t> fencePointsId;  //< Точки забора.
  };
private:
  std::shared_ptr<MainData> mainData; //< Основные данные.
  const std::shared_ptr<Hexagon> ab_hex_; //< Шестиугольник, к которому принадлежат точки a и b.
  const std::shared_ptr<Hexagon> cd_hex_; //< Шестиугольник, к которому принадлежат точки c и d.
};