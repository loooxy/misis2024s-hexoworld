#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>
#include <hexoworld/hexoworld.hpp>

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
  /// \return Шесть точек видимого шестиугольника(0-5я точка) и центр(6я точка).
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
    uint32_t row; ///< Номер строки.
    uint32_t col; ///< Номер столбца.
  };

  /// \brief Класс-синглтон, хранящий точки, которые требуются несколько раз.
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
  struct Hexagon : Object {
    bool is_hexagon() const { return true; }

    /// \brief Конструктор по умолчанию.
    Hexagon() = default;

    /// \brief Создание пустого шестиугольника
    /// \param hexoworld Мир к которому принадлежит шестиугольник.
    Hexagon(Hexoworld& hexoworld) : world(hexoworld) {}

    /// \brief Создание шестиугольника.
    /// \param hexoworld Мир к которому принадлежит шестиугольник.
    /// \param big_size Радиус области покрытия.
    /// \param small_size Радиус отображаемого шестиугольника.
    /// \param center Центр шестиугольника.
    /// \param pointDirection Направление от центра на вершину.
    /// \param floatDirection Направление от центра на центр плоской грани.
    /// !!! должно быть перпендикулярно pointDirection !!!
    Hexagon(Hexoworld& hexoworld,
      float big_size, float small_size, Eigen::Vector3d center,
      Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection);

    /// \brief Соединение точек с множеством всех точек.
    /// \param ptr Указатель на наш шестиугольник.
    void connect_points(std::shared_ptr<Hexagon> ptr);

    /// \brief Получить точки видимого шестиугольника
    std::vector<Eigen::Vector3d> get_points();

    /// \brief Вывести вершины и треугольники
    /// \param Vertices Куда выводить вершины.
    /// \param TriList Куда выводить треугольники.
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList) const;

    std::vector<uint32_t> innerPointsId; ///< Id отображаемых точек.
    std::vector<uint32_t> outerPointsId; ///< Id точек покрытия
    Eigen::Vector3d center; ///< центр шестиугольника
    std::vector<Eigen::Vector3d> innerPoints; ///< Отображаемые точки.
    std::vector<Eigen::Vector3d> outerPoints; ///< Точки покрытия
    Hexoworld& world; ///< мир к которому принадлежит шестиугольник.
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

    /// \brief Вывести вершины и треугольники, на который треангулируется треугольник.
    /// \param Vertices Куда выводить вершины.
    /// \param TriList Куда выводить треугольники.
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
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
    /// \param Vertices Куда выводить вершины.
    /// \param TriList Куда выводить треугольники.
    void print_stair(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
      Eigen::Vector3d a_goal,
      Eigen::Vector3d b_goal,
      Eigen::Vector3d c_goal,
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList) const;

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

    /// \brief Оператор сравнения.
    /// Нужен для составления set и map прямоугольников.
    /// \param rhs Прямоугольник для сравнения.
    /// \return Результат сравнения.
    bool operator< (const BorderRectangle& rhs) const;

    /// \brief Вывести вершины и треугольники, на который треангулируется прямоугольник.
    /// \param Vertices Куда выводить вершины.
    /// \param TriList Куда выводить треугольники.
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList) const;

    uint32_t AId; ///< Id точки a
    uint32_t BId; ///< Id точки b
    uint32_t CId; ///< Id точки c
    uint32_t DId; ///< Id точки d
  private:
    Hexoworld& world; ///< Мир к которому принадлежит прямоугольник.
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
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  static void printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
    std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList);

  /// \brief Вывод треугольника.
  /// \param a Точка a
  /// \param b Точка b
  /// \param c Точка c
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  static void printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList);

  std::map<Coord, std::shared_ptr<Hexagon>> grid_; ///< Сетка.
  std::set<Triangle> triangles; ///< Все межшестиугольные треугольники.
  std::set<BorderRectangle> rectangles; ///< Все межшестиугольные прямоугольники.
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
