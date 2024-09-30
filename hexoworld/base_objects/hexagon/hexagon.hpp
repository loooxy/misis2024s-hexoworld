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
  int32_t get_height() const;

  /// \brief Создать реку в шестиугольнике.
  /// \param in Номер грани входа реки. 
  /// Если -1, то река начинается в этом шестиугольнике.
  /// \param out Номер грани выхода реки. 
  /// Если -1, то река заканчивается в этом шестиугольнике.
  /// \return Точки-берега.
  /// чётные точки - левый берег, нечётные - правый берег.
  std::vector<Eigen::Vector3d> make_river(int32_t in, int32_t out);

  /// \brief Сделать затопление.
  void add_flooding(int32_t height);
  void del_flooding();

  /// \brief Сделать дорогу.
  /// \param ind_roads Грани, через которые идут дороги.
  void make_road(std::vector<uint32_t> ind_roads);

  /// \brief Добавить дорогу.
  /// \param ind_road Новая грань, через которую идёт дорога.
  void make_road(uint32_t ind_road);

  void del_road();
  void del_road(uint32_t ind_road);

  /// \brief Создание фермы.
  void make_farm();
  void del_farm();

  void show_numbers();
  void hide_numbers();

  /// \brief Вывести треугольники
  /// \param TriList Куда выводить треугольники.
  void print_in_triList(std::vector<uint32_t>& TriList);

  /// \brief Раскрашивание точек.
  void colorize_points();

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
    std::vector<IdType> polygonPointsId; ///< Id точек шестиугольника.
    std::vector<std::vector<IdType>> extraPointsId; ///< Id дополнительных точек
    IdType centerId; //< Id центра.
    uint32_t gen_init; //< Число, которым инициализируется генератор случайных чисел.
    int32_t dirFarm = -1; //< Направление фермы или -1 если её нет.
    int32_t height = 0;
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

    /// \brief Получение Id точек обычного каркаса.
    /// \return Массив Id-шников.
    std::vector<IdType> get_pointsId() const;

    /// \brief Получить точки обычного каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Выравнивание грани.
    /// \param ind_edge Номер грани. 
    void normalize_edge(uint32_t ind_edge);

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;
  private:
    /// \brief Инициализация точек.
    /// \param center Центр шестиугольника.
    /// \param center_ Центр шестиугольника.
    /// \param polygonPoints_ Внешние точки шестиугольника.
    /// \param extraPoints_ Точки на гранях.
    void init_points(Eigen::Vector3d center, 
      Eigen::Vector3d& center_, 
      std::vector<Eigen::Vector3d>& polygonPoints_,
      std::vector<std::vector<Eigen::Vector3d>>& extraPoints_);

    /// \brief Добавление случайной компоненты к вершинам.
    /// \param center_ Центр шестиугольника.
    /// \param polygonPoints_ Внешние точки шестиугольника.
    /// \param extraPoints_ Точки на гранях.
    void random_move_points(Eigen::Vector3d& center_,
      std::vector<Eigen::Vector3d>& polygonPoints_,
      std::vector<std::vector<Eigen::Vector3d>>& extraPoints_);

    /// \brief Инициализация Id точек.
    /// \param center_ Центр шестиугольника.
    /// \param polygonPoints_ Внешние точки шестиугольника.
    /// \param extraPoints_ Точки на гранях.
    void init_ids(Eigen::Vector3d& center_,
      std::vector<Eigen::Vector3d>& polygonPoints_,
      std::vector<std::vector<Eigen::Vector3d>>& extraPoints_);
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
    /// \brief Получить Id-шники точек.
    /// \return Массив Id-шников.
    std::vector<IdType> get_pointsId() const;

    /// \brief Получить точки дна, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<IdType> get_floor_pointsId() const;

    /// \brief Получить точки берега, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<IdType> get_shore_pointsId() const;

    /// \brief Получить точки воды, принадлежащие каркасу реки.
    /// \return Массив точек.
    std::vector<IdType> get_water_pointsId() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(std::vector<uint32_t>& TriList) const;

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

    std::vector<std::vector<IdType>> radial_points; ///< Радиальные точки
    std::vector<std::vector<IdType>> middle_points; ///< Точки посередине грани
    std::vector<IdType> floor_points; ///< Точки дна.
    std::vector<IdType> shore_points; ///< Точки берега.
    std::vector<IdType> water_points; //< Точки воды.
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
    /// \brief Получить Id-шники точек.
    /// \return Массив Id-шников.
    std::vector<IdType> get_pointsId() const;

    /// \brief Вывести треугольники, на которые треангулируется каркас.
    /// \param TriList Куда выводить треугольники. 
    void print_in_triList(std::vector<uint32_t>& TriList) const;

    double water_level; //< Уровень воды.
    std::vector<IdType> waterPoints; //< Точки воды.
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
    void del_road(uint32_t ind);

    /// \brief Установить высоту.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить точки каркаса.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;
    /// \brief Получить Id-шники точек.
    /// \return Массив Id-шников.
    std::vector<IdType> get_pointsId() const;

    /// \brief Получить внутренние заборы.
    /// \return Массив пар точек (начало и конец).
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> get_inner_fence();
    /// \brief Получить внешние заборы.
    /// \return Массив пар точек (начало и конец).
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> get_outer_fence();

    /// \brief Вывести треугольники, на которые треангулируется каркас.
    /// \param TriList Куда выводить треугольники. 
    void print_in_triList(std::vector<uint32_t>& TriList) const;

    std::vector<IdType> radial_points; //< Радиальные точки
    std::vector<IdType> middle_points; //< Точки посередине грани
    std::vector<IdType> crossroads;    //< Точки шестиугольника перекрёстка.
    IdType centerId; //< сентер
    std::map<uint32_t, std::pair<IdType, IdType>> inputs; //< Въезды на шестиугольник.
    std::map<uint32_t, std::pair<IdType, IdType>> endOfFence; //< Концы внешних заборов.
    std::map<uint32_t, std::pair<IdType, IdType>> middleFence;//< Заборы посередине.
    std::vector<bool> isRoad; //< Есть ли дорога по направлению i.
  private:
    /// \brief Инициализация дороги.
    /// \param ind Направление дороги.
    void init_road(uint32_t ind);

    /// \brief Поиск точки пересечения двух отрезков.
    /// \param start1 Начало первого отрезка.
    /// \param end1 Конец первого отрезка.
    /// \param start2 Начало второго отрезка.
    /// \param end2 Конец второго отрезка.
    /// \return Точка их пересечения.
    Eigen::Vector3d cuts_intersection(
      Eigen::Vector3d start1, Eigen::Vector3d end1, 
      Eigen::Vector3d start2, Eigen::Vector3d end2);
  };

  std::shared_ptr<MainData> mainData; //< Основные данные о каркасе.
  const Coord coord; ///< Координаты шестиугольника.
private:
  /// \brief Инициализация инвенторя.
  void init_inventory();

  /// \brief Инициализация координат на шестиугольниках.
  void init_number();

  void init_boundary_walls();

  bool is_numbers_show = false;
};
