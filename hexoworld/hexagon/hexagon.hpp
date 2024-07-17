#pragma once
#include <hexoworld/hexoworld.hpp>

/// \brief Структура шестиугольник.
struct Hexoworld::Hexagon : public Object {
  /// \brief Конструктор по умолчанию удалён.
  Hexagon() = delete;

  /// \brief Создание шестиугольника.
  /// \param hexoworld Мир к которому принадлежит шестиугольник.
  /// \param center Центр шестиугольника.
  /// \param coord Координаты шестиугольника.
  Hexagon(Hexoworld& hexoworld, Eigen::Vector3d center, Coord coord);

  /// \brief Создать реку в шестиугольнике.
  /// \param in Номер грани входа реки. 
  /// Если -1, то река начинается в этом шестиугольнике.
  /// \param out Номер грани выхода реки. 
  /// Если -1, то река заканчивается в этом шестиугольнике.
  /// \return Точки-берега.
  /// чётные точки - левый берег, нечётные - правый берег.
  std::vector<Eigen::Vector3d> make_river(int32_t in, int32_t out);

  /// \brief Вывести треугольники
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint16_t>& TriList);

  /// \brief Шестиугольный отрисовщик.
  class HexagonDrawer : public Drawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит каркас.
    /// \param color Цвет.
    HexagonDrawer(Object* object,
      Eigen::Vector4i color);
  };

  /// \brief Отрисовщик рек.
  class RiverDrawer : public Drawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    /// \param color Основной цвет отрисовщика.
    RiverDrawer(Object* object,
      Eigen::Vector4i color);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Основные данные о каркасе.
  struct MainData {
    std::vector<uint32_t> innerPointsId; ///< Id отображаемых точек.
    std::vector<uint32_t> outerPointsId; ///< Id точек покрытия
    std::vector<std::vector<uint32_t>> extraPointsId; ///< Id дополнительных точек
    Eigen::Vector3d center; ///< центр шестиугольника
    std::vector<Eigen::Vector3d> innerPoints; ///< Отображаемые точки.
    std::vector<Eigen::Vector3d> outerPoints; ///< Точки покрытия
    std::vector<std::vector<Eigen::Vector3d>> extraPoints; ///< Дополнительные точки
    uint32_t gen_init; //< Число, которым инициализируется гениратор случайных чисел.
  };

  /// \brief Каркас шестиугольника.
  struct HexagonFrame : public Frame {
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    /// \param center Центр шестиугольника.
    /// \param mainData Основные данные.
    HexagonFrame(Object* base, Eigen::Vector3d center, std::shared_ptr<MainData> mainData = nullptr);

    /// \brief Установить высоту объекту типа.
    /// \param height Новая высота.
    virtual void set_height(int32_t height);

    /// \brief Получить все точки каркаса.
    /// \return Массив точек.
    virtual std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Установить высоту точке.
    /// \param point Точка.
    /// \param height Новая высота.
    void set_new_height_to_point(Eigen::Vector3d& point, int32_t height);

    /// \brief Получить номер грани между двумя вершинами.
    /// \param vertex1 Первая вершина.
    /// \param vertex2 Вторая вершина.
    /// \return Номер грани.
    uint32_t get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2);

    std::shared_ptr<MainData> mainData; //< Основные данные о каркасе.
  };

  /// \brief Обычный каркас.
  struct UsualFrame : public HexagonFrame {
    /// \brief Создает обычный шестиугольник.
    /// \param base Шестиугольник к которому принадлежит каркас.
    /// \param center Центр шестиугольника.
    /// \param mainData Основные данные.
    UsualFrame(Object* base, Eigen::Vector3d center, std::shared_ptr<MainData> mainData = nullptr);

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint16_t>& TriList) const;
  };

  /// \brief Речной каркас шестиугольника.
  struct RiversFrame : HexagonFrame {
    /// \brief Создание реки.
    /// \param base Шестиугольник, к которому принадлежит каркас.
    /// \param center Центр шестиугольника.
    /// \param in Номер грани входа реки. 
    /// Если -1, то река начинается в этом шестиугольнике.
    /// \param out Номер грани выхода реки. 
    /// Если -1, то река заканчивается в этом шестиугольнике.
    /// \param mainData Основные данные каркаса.
    RiversFrame(Object* base, Eigen::Vector3d center, int32_t in, int32_t out,
      std::shared_ptr<MainData> mainData = nullptr);

    /// \brief Устанавливает высоту шестиугольнику.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить все точки, принадлежащие каркасу шестиугольника.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Получить точки дна, принадлежащие каркасу шестиугольника.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_floor_points() const;

    /// \brief Получить точки берега, принадлежащие каркасу шестиугольника.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_shore_points() const;

    /// \brief Получить точки воды, принадлежащие каркасу шестиугольника.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_water_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint16_t>& TriList) const;

  private:

    /// \brief Создать исток/устье.
    /// \param edge Грань, через которую вытекает/втекает река.
    void make_river_begin_end(uint32_t edge);

    /// \brief Создать прямую реку.
    /// \param in Грань, через которую втекает река.
    /// \param out Грань, через которую вытекает река.
    void make_river_directly(uint32_t in, uint32_t out);

    /// \brief Создать реку с поворотом через треугольник.
    /// \param in Грань, через которую втекает река.
    /// \param out Грань, через которую вытекает река.
    void make_river_angle_2(uint32_t in, uint32_t out);

    /// \brief Создать реку с поворотом в соседний треугольник.
    /// \param in Грань, через которую втекает река.
    /// \param out Грань, через которую вытекает река.
    void make_river_angle_1(uint32_t in, uint32_t out);

    /// \brief Опустить точку.
    /// \param point Точка.
    /// \param deep Величина от 0 до 1, относительная глубина.
    void omit_point(Eigen::Vector3d& point, double deep = 1);

    std::vector<std::vector<Eigen::Vector3d>> radial_points; ///< Радиальные точки
    std::vector<std::vector<Eigen::Vector3d>> middle_points; ///< Точки посередине грани
    std::vector<Eigen::Vector3d> floor_points; ///< Точки дна.
    std::vector<Eigen::Vector3d> shore_points; ///< Точки берега.
    std::vector<Eigen::Vector3d> water_points;
    int32_t in_;  ///< Грань, через которую втекает река.
    int32_t out_; ///< Грань, через которую вытекает река.
    double deep_; ///< Абсолютная глубина реки.
  };
private:
  const Coord coord; ///< Координаты шестиугольника.
};
