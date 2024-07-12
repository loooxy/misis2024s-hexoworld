#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>
#include <hexoworld/hexoworld.hpp>

/// \brief Класс шестиугольной сетки.
class Hexoworld::HexagonGrid {
public:
  /// \brief Конструктор по умолчанию запрещён, так как необходимы параметры сетки.
  HexagonGrid() = delete;

  /// \brief Создание сетки.
  /// \param size Мир, к которому принадлежит шестиугольная сетка.
  /// \param size Радиус шестиугольника покрытия.
  /// \param origin Начало координат.
  /// \param row_direction Направление увеличения номера строки.
  /// \param сol_direction Направление увеличения номера столбца.
  /// !!! должно быть перпендикулярно row_direction !!!
  /// \param height_step Высота между соседними уровнями.
  /// \param n_terraces_on_height_step Количество террас между соседними уровнями.
  HexagonGrid(Hexoworld& hexoworld,
    float size, Eigen::Vector3d origin,
    Eigen::Vector3d row_direction, Eigen::Vector3d сol_direction,
    float height_step = 1.0f,
    uint32_t n_terraces_on_height_step = 2);

  /// \brief Добавить шестиугольник.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  void add_hexagon(uint32_t row, uint32_t col);

  /// \brief Добавить реку
  /// \param hexs Координаты {row, col} шестиугольников, по которым течёт река.
  /// \return Точки-берега.
  /// чётные точки - правый берег, нечётные - левый берег.
  std::vector<Eigen::Vector3d> add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs);

  /// \brief Вывести вершины и треугольники, на который треангулируется сетка.
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  /// \brief Установить высоту шестиугольнику.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param height Новая высота.
  void set_height(int row, int col, int32_t height);

  /// \brief Получить точки видимого шестиугольника по координатам
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \return Видимые точки шестиугольника.
  std::vector<Eigen::Vector3d> get_hex_points(int row, int col);

private:
  /// \brief Базовая структура всех объектов
  struct Object {
    /// \brief Конструктор по умолчанию.
    Object() = default;

    virtual bool is_hexagon() const { return false; }
    virtual bool is_triangle() const { return false; }
    virtual bool is_rectangle() const { return false; }
  };

  /// \brief Структура координат
  struct Coord {
    /// \brief Конструктор по умолчанию.
    Coord() = default;

    /// \brief Конструктор от номера строки и номера столбца.
    /// \param row Номер строки.
    /// \param col Номер столбца.
    Coord(uint32_t row, uint32_t col) : row(row), col(col) {}

    /// \brief Сравнение координат.
    /// \param rhs Координаты для сравнения.
    /// \return Результат сравнения
    bool operator< (const Coord& rhs) const
    {
      return (row < rhs.row) ||
        ((row == rhs.row) && (col < rhs.col));
    }
    int32_t row; ///< Номер строки.
    int32_t col; ///< Номер столбца.
  };

  /// \brief Класс-синглтон, хранящий все точки.
  class Points {
  public:
    /// \brief Конструктор по умолчанию.
    Points() = default;

    /// \brief Получение экземпляра.
    /// \return Экземпляр.
    static Points& get_instance() {
      static Points instance;
      return instance;
    }

    /// \brief Соединение точки с объектом.
    /// \param p Точка.
    /// \param object Объект.
    /// \return Id точки
    uint32_t connect_point_with_object(Eigen::Vector3d p,
      std::shared_ptr<Object> object);

    /// \brief Проверка наличия точки.
    /// \param p Точка.
    /// \return true - если точка есть, иначе false
    bool in_points(Eigen::Vector3d p) const;

    /// \brief Получение Id точки.
    /// \param p Точка.
    /// \return Id.
    uint32_t get_id_point(Eigen::Vector3d p);

    /// \brief Получение объектов, связанных с точкой.
    /// \param id Id точки.
    /// \return Объекты.
    std::vector<std::shared_ptr<Object>> get_objects(uint32_t id);

    /// \brief Получение точки по Id.
    /// \param id Id.
    /// \return Точка.
    Eigen::Vector3d get_point(uint32_t id) const;

    /// \brief Присвоить Id новую точку.
    /// \param id Id.
    /// \param new_point Новая точка.
    void update_point(uint32_t id, Eigen::Vector3d new_point);

    /// \brief Записать все точки в массив точек.
    /// \param Vertices Массив точек.
    void print_in_vertices(std::vector<PrintingPoint>& Vertices) {
      for (const Eigen::Vector3d& p : id_to_point)
        Vertices.push_back(p);
    }
  private:
    std::map<Eigen::Vector3d, uint32_t, EigenVector3dComp> point_to_id;
    std::vector<Eigen::Vector3d> id_to_point;
    std::vector<std::vector<std::shared_ptr<Object>>> id_to_objects;
  };

  /// \brief Структура шестиугольник.
  struct Hexagon;

  /// \brief Тип шестиугольника.
  struct HexagonType {
    virtual bool is_usual() { return false; }
    virtual bool is_river() { return false; }

    /// \brief Конструктор объекта типа.
    /// \param hexagon Шестиугольник, к которому относится объект типа.
    HexagonType(Hexagon& hexagon) : base(hexagon) {}

    /// \brief Установить высоту объекту типа.
    /// \param height Новая высота.
    virtual void set_height(int32_t height){}

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(
      std::vector<uint16_t>& TriList) const = 0;
  protected:
    Hexagon& base; ///< Шестиугольник к которому относится этот обект типа.
  };

  /// \brief Обычный шестиугольник.
  struct UsualType : HexagonType {
    virtual bool is_usual() { return true; }

    /// \brief Создает обычный шестиугольник.
    /// \param hexagon Шестиугольник к которому принадлежит объект типа.
    UsualType(Hexagon& hexagon);

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(
      std::vector<uint16_t>& TriList) const;
  };

  /// \brief Речной тип шестиугольника.
  struct RiversType : HexagonType {
    bool is_river() { return true; }

    /// \brief Создание реки.
    /// \param hexagon Шестиугольник к которому принадлежит объект типа.
    /// \param in Номер грани входа реки. 
    /// Если -1, то река начинается в этом шестиугольнике.
    /// \param out Номер грани выхода реки. 
    /// Если -1, то река заканчивается в этом шестиугольнике.
    RiversType(Hexagon& hexagon, int32_t in, int32_t out);

    /// \brief Устанавливает высоту шестиугольнику.
    /// \param height Новая высота.
    void set_height(int32_t height);

    /// \brief Получить точки, принадлежащие объекту типа шестиугольника.
    /// \return Массив точек.
    std::vector<Eigen::Vector3d> get_points() const;

    /// \brief Вывести треугольники.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(
      std::vector<uint16_t>& TriList) const;
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
    int32_t in_;  ///< Грань, через которую втекает река.
    int32_t out_; ///< Грань, через которую вытекает река.
    double deep_; ///< Абсолютная глубина реки.
  };

  /// \brief Структура шестиугольник.
  struct Hexagon : Object {
    bool is_hexagon() const { return true; }

    /// \brief Конструктор по умолчанию удалён.
    Hexagon() = delete;

    /// \brief Создание шестиугольника.
    /// \param hexoworld Мир к которому принадлежит шестиугольник.
    /// \param big_size Радиус области покрытия.
    /// \param small_size Радиус отображаемого шестиугольника.
    /// \param center Центр шестиугольника.
    /// \param pointDirection Направление от центра на вершину.
    /// \param floatDirection Направление от центра на центр плоской грани.
    /// !!! должно быть перпендикулярно pointDirection !!!
    /// \param coord Координаты шестиугольника.
    Hexagon(Hexoworld& hexoworld,
      float big_size, float small_size, Eigen::Vector3d center,
      Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection,
      Coord coord);

    /// \brief Соединение точек с множеством всех точек.
    /// \param ptr Указатель на наш шестиугольник.
    void connect_points(std::shared_ptr<Hexagon> ptr);

    /// \brief Установить высоту шестиугольнику
    /// \param height Новая высота
    void set_height(int32_t height);

    /// \brief Создать реку в шестиугольнике.
    /// \param in Грань, через которую втекает река.
    /// \param out Грань, через которую вытекает река.
    /// \return Точки-берега.
    /// чётные точки - правый берег, нечётные - левый берег.
    std::vector<Eigen::Vector3d> make_river(int32_t in, int32_t out);

    /// \brief Получить точки видимого шестиугольника
    std::vector<Eigen::Vector3d> get_points();
    
    uint32_t get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2);

    /// \brief Вывести треугольники
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(
      std::vector<uint16_t>& TriList) const;

    friend RiversType;

    std::vector<uint32_t> innerPointsId; ///< Id отображаемых точек.
    std::vector<uint32_t> outerPointsId; ///< Id точек покрытия
    std::vector<std::vector<uint32_t>> extraPointsId; ///< Id дополнительных точек
    Eigen::Vector3d center; ///< центр шестиугольника
    mutable std::vector<Eigen::Vector3d> innerPoints; ///< Отображаемые точки.
    mutable std::vector<Eigen::Vector3d> outerPoints; ///< Точки покрытия
    mutable std::vector<std::vector<Eigen::Vector3d>> extraPoints; ///< Дополнительные точки
    std::shared_ptr<HexagonType> hexagonType; ///< Тип шестиугольника.
    Hexoworld& world; ///< мир к которому принадлежит шестиугольник.
    const Coord coord; ///< Координаты шестиугольника.
  private:
    /// \brief Установить высоту точке.
    /// \param point Точка.
    /// \param height Новая высота.
    void set_new_height_to_point(Eigen::Vector3d& point, int32_t height);

    /// \brief сместить точки в случайном направлении. Шестиугольник остаётся плоским.
    void disturb_the_points();

    uint32_t gen_init;
    bool was_init_gen = false;
  };

  /// \brief Класс треугольник.
  class Triangle : Object {
  public:
    bool is_triangle() const { return true; }

    /// \brief Конструктор по умолчанию.
    Triangle() = default;

    /// \brief Создать треугольник.
    /// \param hexoworld Мир к которому принадлежит треугольник.
    /// \param a Точка a.
    /// \param b Точка b.
    /// \param c Точка с.
    Triangle(Hexoworld& hexoworld,
      Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c);

    /// \brief Создать треугольник.
    /// \param hexoworld Мир к которому принадлежит треугольник.
    /// \param aId Id точки a.
    /// \param bId Id точки b.
    /// \param cId Id точки c.
    Triangle(Hexoworld& hexoworld,
      uint32_t aId, uint32_t bId, uint32_t cId);

    /// \brief Оператор сравнения.
    /// Нужен для составления set и map треугольников.
    /// \param rhs Треугольник для сравнения.
    /// \return Результат сравнения.
    bool operator< (const Triangle& rhs) const;

    /// \brief Вывести треугольники, на который треангулируется треугольник.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(
      std::vector<uint16_t>& TriList) const;

    uint32_t AId; ///< Id точки a
    uint32_t BId; ///< Id точки b
    uint32_t CId; ///< Id точки c
  private:
    /// \brief Вывод террас.
    /// \param a Точка a треугольника.
    /// \param b Точка b треугольника.
    /// \param c Точка c треугольника.
    /// \param a_goal Мнимая позиция точки a.
    /// \param b_goal Мнимая позиция точки b.
    /// \param c_goal Мнимая позиция точки c.
    /// \param TriList Куда выводить треугольники.
    /// \param cliff Точка от которой не идёт террас.
    /// При 0 террасы идут от всех точек.
    /// При 1 террасы идут только между b и с.
    /// При 2 террасы идут только между a и с.
    /// При 3 террас не будет.
    void print_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
      Eigen::Vector3d a_goal,
      Eigen::Vector3d b_goal,
      Eigen::Vector3d c_goal,
      std::vector<uint16_t>& TriList,
      uint8_t cliff = 0) const;

    Hexoworld& world; ///< Мир к которому принадлежит треугольник.
  };

  /// \brief Класс прямоугольник между двух шестиугольников 
  class BorderRectangle : Object {
  public:
    bool is_rectangle() const { return true; }

    /// \brief Конструктор по умолчанию
    BorderRectangle() = default;

    /// \brief Создание прямоугольника.
    /// \param hexoworld Мир к которому принадлежит прямоугольник.
    /// \param a Точка a.
    /// \param b Точка b.
    /// \param c Точка c.
    /// \param d Точка d.
    BorderRectangle(Hexoworld& hexoworld,
      Eigen::Vector3d a, Eigen::Vector3d b,
      Eigen::Vector3d c, Eigen::Vector3d d);

    /// \brief Создание прямоугольника.
    /// \param hexoworld Мир к которому принадлежит прямоугольник.
    /// \param aId Id точки a.
    /// \param bId Id точки b.
    /// \param cId Id точки c.
    /// \param dId Id точки d.
    BorderRectangle(Hexoworld& hexoworld,
      uint32_t aId, uint32_t bId,
      uint32_t cId, uint32_t dId);

    /// \brief Установить отрисовку террас.
    /// \param side Если разница высот 1, то при 
    /// side == 3 террасы отрисуются со всех сторон.
    /// side == 2 террасы отрисуются только справа.
    /// side == 1 террасы отрисуются только слева.
    /// side == 0 террасы не отрисуются.
    void set_terraces_side(uint8_t side) { terraces_side = side; }

    /// \brief Оператор сравнения.
    /// Нужен для составления set и map прямоугольников.
    /// \param rhs Прямоугольник для сравнения.
    /// \return Результат сравнения.
    bool operator< (const BorderRectangle& rhs) const;

    /// \brief Вывести треугольники, на который треангулируется прямоугольник.
    /// \param TriList Куда выводить треугольники.
    void print_in_triList(
      std::vector<uint16_t>& TriList) const;

    uint32_t AId; ///< Id точки a
    uint32_t BId; ///< Id точки b
    uint32_t CId; ///< Id точки c
    uint32_t DId; ///< Id точки d
  private:
    Hexoworld& world; ///< Мир к которому принадлежит прямоугольник.
    uint8_t terraces_side = 3;///< сторона отрисовки террас
  };

  /// \brief Соединить точку с объектом.
  /// \param point Точка.
  /// \param object Объект.
  /// \return Id точки.
  static uint32_t connect(Eigen::Vector3d point, std::shared_ptr<HexagonGrid::Object> object);

  /// \brief Вывод прямоугольника.
  /// \param a Первая пара точек.
  /// \param b Вторая пара точек.
  /// a.first-b.first должно быть параллельно a.second-b.second
  /// \param TriList Куда выводить треугольники.
  static void printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
    std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
    std::vector<uint16_t>& TriList);

  /// \brief Вывод треугольника.
  /// \param a Точка a
  /// \param b Точка b
  /// \param c Точка c
  /// \param TriList Куда выводить треугольники.
  static void printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
    std::vector<uint16_t>& TriList);

  /// \brief Возвращает номер грани a, которая ведёт к b.
  /// \param a Координата первого шестиугольника.
  /// \param b Координата второго шестиугольника.
  /// \return Номер грани.
  uint32_t get_ind_direction(Coord a, Coord b);

  /// \brief Создать отсортированную пару координат.
  /// \param a Первая координата.
  /// \param b Вторая координата.
  /// \return Пара.
  std::pair<Coord, Coord> pair_coords(Coord a, Coord b);
  
  /// \brief Создать реку в шестиугольнике.
  /// \param before Предыдущий шестиугольник.
  /// \param pos Позиция текущего шестиугольника.
  /// \param next Позиция следующего шестиугольника.
  /// \return Точки-берега.
  /// чётные точки - правый берег, нечётные - левый берег.
  std::vector<Eigen::Vector3d> add_river_in_hex(Coord before, Coord pos, Coord next);

  std::map<Coord, std::shared_ptr<Hexagon>> grid_; ///< Сетка.
  std::set<Triangle> triangles; ///< Все межшестиугольные треугольники.
  std::map<std::pair<Coord, Coord>, 
    std::vector<BorderRectangle>> rectangles; ///< Все межшестиугольные прямоугольники.
  Eigen::Vector3d rowDirection_; ///< Направление увеличения номера строки.
  Eigen::Vector3d colDirection_; ///< Направление увеличения номера столбца.
  Eigen::Vector3d heightDirection_;///< Направление вверх.
  Eigen::Vector3d origin_; ///< Начало координат.
  float size_; ///< Радиус шестиугольника покрытия.
  float innerSize_; ///< Радиус внутреннего шестиугольника.
  float heightStep_; ///< Расстояние между соседними уровнями. 
  uint32_t nTerracesOnHeightStep_;///< Число террас между соседними уровнями.
  Hexoworld& world; ///< Мир, к которому принадлежит шестиугольная сетка.
};
