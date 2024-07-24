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

  /// \brief Установить высоту шестиугольнику.
  /// \param height Новая высота.
  void set_height(int32_t height);

  /// \brief Создать реку в шестиугольнике.
  /// \param in Номер грани входа реки. 
  /// Если -1, то река начинается в этом шестиугольнике.
  /// \param out Номер грани выхода реки. 
  /// Если -1, то река заканчивается в этом шестиугольнике.
  /// \return Точки-берега.
  /// чётные точки - левый берег, нечётные - правый берег.
  std::vector<Eigen::Vector3d> make_river(int32_t in, int32_t out);

  /// \brief Сделать затопление.
  void make_flooding();

  /// \brief Сделать дорогу.
  /// \param ind_roads Грани, через которые идут дороги.
  void make_road(std::vector<uint32_t> ind_roads);

  /// \brief Добавить дорогу.
  /// \param ind_road Новая грань, через которую идёт дорога.
  void make_road(uint32_t ind_road);

  /// \brief Вывести треугольники
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint16_t>& TriList);

  /// \brief Шестиугольный отрисовщик.
  class HexagonDrawer : public Drawer<Object>{
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит отрисовщик.
    HexagonDrawer(Object* object);
  };

  /// \brief Обычный обязательный отрисовщик.
  class UsualDrawer : public HexagonDrawer {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит отрисовщик.
    /// \param color Основной цвет объекта.
    UsualDrawer(Object* base, Eigen::Vector4i color);

    /// \brief Установить основной цвет объекта.
    /// \param color Цвет.
    void set_color(Eigen::Vector4i color) { color_ = color; }

    /// \brief Получить основной цвет объекта.
    /// \return Цвет.
    Eigen::Vector4i get_color() const { return color_; }

    /// \brief Раскрасить точки.
    void colorize_points();
  private:
    Eigen::Vector4i color_; //< Основной цвет объекта. 
  };

  /// \brief Отрисовщик рек.
  class RiverDrawer : public HexagonDrawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    RiverDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();

    /// \brief Установить новый специальный цвет реки.
    /// \param new_color Новый специальный цвет.
    void set_new_special_color_river(Eigen::Vector4i new_color);

    Eigen::Vector4i special_color_river; //< Специальный цвет реки.
  };

  /// \brief Отрисовщик затопления.
  struct FloodDrawer : public HexagonDrawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    FloodDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Отрисовщик дороги
  struct RoadDrawer : public HexagonDrawer {
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    RoadDrawer(Object* object);

    /// \brief Раскрасить точки.
    void colorize_points();
  };

  /// \brief Основные данные о каркасе.
  struct MainData {
    std::vector<uint32_t> polygonPointsId; ///< Id точек шестиугольника.
    std::vector<std::vector<uint32_t>> extraPointsId; ///< Id дополнительных точек
    Eigen::Vector3d center; ///< центр шестиугольника
    std::vector<Eigen::Vector3d> polygonPoints; ///< Точки шестиугольника.
    std::vector<std::vector<Eigen::Vector3d>> extraPoints; ///< Дополнительные точки
    uint32_t gen_init; //< Число, которым инициализируется генератор случайных чисел.
  };

  /// \brief Каркас шестиугольника.
  struct HexagonFrame : public Frame<Object>{
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    HexagonFrame(Object* base);

    /// \brief Установить высоту.
    /// \param height Новая высота.
    virtual void set_height(int32_t height) = 0;
  };

  /// \brief Обычный каркас.
  struct UsualFrame : public HexagonFrame {
    /// \brief Создает обычный шестиугольник.
    /// \param base Шестиугольник к которому принадлежит каркас.
    /// \param center Центр шестиугольника.
    UsualFrame(Object* base, Eigen::Vector3d center);

    /// \brief Установить высоту.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить точки обычного каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint16_t>& TriList) const;
  };

  /// \brief Речной каркас.
  struct RiversFrame : public HexagonFrame {
    /// \brief Создание реки.
    /// \param base Объект, к которому принадлежит каркас.
    /// \param in Номер грани входа реки. 
    /// Если -1, то река начинается в этом шестиугольнике.
    /// \param out Номер грани выхода реки. 
    /// Если -1, то река заканчивается в этом шестиугольнике.
    /// \param deep Глубина реки.
    RiversFrame(Object* base, int32_t in, int32_t out, double deep = 0.2);

    /// \brief Устанавливает высоту шестиугольнику.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить все точки, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Получить точки дна, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_floor_points() const;

    /// \brief Получить точки берега, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_shore_points() const;

    /// \brief Получить точки воды, принадлежащие каркасу реки.
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
    std::vector<Eigen::Vector3d> water_points; //< Точки воды.
    int32_t in_;  ///< Грань, через которую втекает река.
    int32_t out_; ///< Грань, через которую вытекает река.
    double deep_; ///< Абсолютная глубина реки.
  };

  /// \brief Каркас затопления.
  struct FloodFrame : public HexagonFrame {
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    /// \param water_level Уровень воды.
    FloodFrame(Object* base, double water_level);

    /// \brief Установить новую высоту.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить точки каркаса затопления.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники, на которые треангулируется каркас.
    /// \param TriList Куда выводить треугольники. 
    void print_in_triList(std::vector<uint16_t>& TriList) const;

    double water_level; //< Уровень воды.
    std::vector<Eigen::Vector3d> waterPoints; //< Точки воды.
  };

  /// \brief Каркас дороги.
  struct RoadFrame : public HexagonFrame {
    /// \brief Конструктор.
    /// \param object Объект, к которому принадлежит каркас.
    /// \param edges Грани, через которые пролегает дорога.
    RoadFrame(Object* object, std::vector<uint32_t> edges);

    /// \brief Добавить дорогу.
    /// \param ind Грань, через которую проходит дорога.
    void add_road(uint32_t ind);

    /// \brief Установить высоту.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;
    /// \brief Получить внутренние заборы.
    /// \return Массив пар точек (начало и конец).
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> get_inner_fence();
    /// \brief Получить внешние заборы.
    /// \return Массив пар точек (начало и конец).
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> get_outer_fence();

    /// \brief Вывести треугольники, на которые треангулируется каркас.
    /// \param TriList Куда выводить треугольники. 
    void print_in_triList(std::vector<uint16_t>& TriList) const;

    std::vector<Eigen::Vector3d> radial_points; //< Радиальные точки
    std::vector<Eigen::Vector3d> middle_points; //< Точки посередине грани
    std::vector<Eigen::Vector3d> crossroads;    //< Точки шестиугольника перекрёстка.
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> inputs; //< Въезды на шестиугольник.
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> endOfFence; //< Концы внешних заборов.
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> middleFence;//< Заборы посередине.
    std::vector<bool> isRoad; //< Есть ли дорога по направлению i.
  private:
    /// \brief Инициализация дороги.
    /// \param ind Направление дороги.
    void init_road(uint32_t ind);
  };

  std::shared_ptr<MainData> mainData; //< Основные данные о каркасе.
private:
  /// \brief Инициализация инвенторя.
  void init_inventory();

  const Coord coord; ///< Координаты шестиугольника.
};
