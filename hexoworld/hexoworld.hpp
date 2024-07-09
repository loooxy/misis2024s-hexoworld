#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <Eigen/Dense>
#define PRECISION_DBL_CALC 0.0001 

/// <summary>
/// Класс Color нужен для хранения цвета.
/// </summary>
class Color {
public:
  /// <summary>
  /// Создаёт чёрный непрозрачный цвет.
  /// </summary>
  Color();

  /// <summary>
  /// Создаёт цвет.
  /// </summary>
  /// <param name="red">
  /// Красная компонента цвета
  /// </param>
  /// <param name="blue">
  /// Синяя компонента цвета
  /// </param>
  /// <param name="green">
  /// Зелёная компонента цвета
  /// </param>
  /// <param name="alpha">
  /// Компонента прозрачности цвета
  /// </param>
  /// <param name="n_parts">
  /// Число частей цвета (нужно будет для смешивания).
  /// </param>
  Color(uint8_t red, uint8_t blue,
    uint8_t green, uint8_t alpha = 255, uint32_t n_parts = 1);

  /// <summary>
  /// Создаёт цвет.
  /// </summary>
  /// <param name="abgr">
  /// Цвет в формате alpha-blue-green-red.
  /// </param>
  /// <param name="n_parts">
  /// Число частей цвета (нужно будет для смешивания).
  /// </param>
  Color(uint32_t abgr, uint32_t n_parts = 1);
  
  /// <summary>
  /// Операция + смешивает два цвета в пропорциях n_parts.
  /// </summary>
  /// <param name="rhs">
  /// Цвет, с которым смешиваем.
  /// </param>
  /// <returns>
  /// Новый цвет.
  /// </returns>
  Color operator+ (const Color& rhs) const;

  /// <summary>
  /// Операция - находит цвет, который надо смешать с вычитаемом, чтобы получить исходный цвет.
  /// </summary>
  /// <param name="rhs">
  /// Вычитаемый цвет.
  /// </param>
  /// <returns> </returns>
  Color operator- (const Color& rhs) const;

  /// <summary>
  /// get_abgr() возвращает цвет в формате alpha-blue-green-red.
  /// </summary>
  /// <returns>
  /// Цвет в формате alpha-blue-green-red.
  /// </returns>
  uint32_t get_abgr() const;
private:
  uint32_t abgr_;
  uint32_t n_parts_;
};

/// <summary>
/// Структура точки.
/// </summary>
struct Point
{
  /// <summary>
  /// Создаёт точку с координатами (0, 0, 0) чёрного непрозрачного цвета.
  /// </summary>
  Point();

  /// <summary>
  /// Создаёт точку по позиции и цвету.
  /// </summary>
  /// <param name="pos">
  /// Позиция точки.
  /// </param>
  /// <param name="color">
  /// Цвет точки.
  /// </param>
  Point(Eigen::Vector3d pos, Color color = Color(0, 0, 0));

  /// <summary>
  /// Создаёт точку по координатам и цвету
  /// </summary>
  /// <param name="x">
  /// x координата.
  /// </param>
  /// <param name="y">
  /// y координата.
  /// </param>
  /// <param name="z">
  /// z координата.
  /// </param>
  /// <param name="color">
  /// Цвет точки. 
  /// </param>
  Point(double x, double y, double z, 
    Color color = Color(0, 0, 0));

  /// <summary>
  /// Возвращает точку, смещённую на вектор.
  /// </summary>
  /// <param name="v"> 
  /// Вектор смещения. 
  /// </param>
  /// <returns> 
  /// Полученная точка. 
  /// </returns>
  Point operator+ (Eigen::Vector3d v);

  /// <summary>
  /// Возвращает точку, смещённую на вектор, обратный к данному.
  /// </summary>
  /// <param name="v"> 
  /// Вектор смещения. 
  /// Точка будет смещена на вектор, обратный к данному.
  /// </param>
  /// <returns> 
  /// Полученная точка. 
  /// </returns>
  Point operator- (Eigen::Vector3d v);

  /// <summary>
  /// Смещает точку на вектор. 
  /// </summary>
  /// <param name="v">
  /// Вектор смещения.
  /// </param>
  /// <returns></returns>
  Point& operator+= (Eigen::Vector3d v);

  /// <summary>
  /// Сравнивает позиции точек с точностью PRECISION_DBL_CALC.
  /// </summary>
  /// <param name="rhs">
  /// Точка для сравнения.
  /// </param>
  /// <returns>
  /// Результат сравнения.
  /// </returns>
  bool operator< (const Point& rhs) const;

  /// <summary>
  /// Позиция точки.
  /// </summary>
  Eigen::Vector3d position;

  /// <summary>
  /// Цвет точки.
  /// </summary>
  Color color;
};

/// <summary>
/// Структура формата вывода точки.
/// </summary>
struct PrintingPoint {
  /// <summary>
  /// Создаёт точку в формате вывода. 
  /// </summary>
  /// <param name="p">
  /// Точка.
  /// </param>
  PrintingPoint(Point p);

  float x, y, z;
  uint32_t abgr;
};

/// <summary>
/// Класс шестиугольной сетки.
/// </summary>
class HexagonGrid {
public:
  /// <summary>
  /// Создание сетки.
  /// </summary>
  /// <param name="size">
  /// Радиус шестиугольника покрытия.
  /// </param>
  /// <param name="origin">
  /// Начало координат.
  /// </param>
  /// <param name="row_direction">
  /// Направление увеличения номера строки.
  /// </param>
  /// <param name="сol_direction">
  /// Направление увеличения номера столбца.
  /// !!! должно быть перпендикулярно row_direction !!!
  /// </param>
  /// <param name="n_rows">
  /// Количество строк.
  /// </param>
  /// <param name="n_cols">
  /// Количество столбцов.
  /// </param>
  /// <param name="color">
  /// Цвет сетки.
  /// </param>
  /// <param name="height_step">
  /// Высота между соседними уровнями.
  /// </param>
  /// <param name="n_terraces_on_height_step">
  /// Количество террас между соседними уровнями.
  /// </param>
  HexagonGrid(float size, Eigen::Vector3d origin,
    Eigen::Vector3d row_direction, Eigen::Vector3d сol_direction,
    uint32_t n_rows = 0, uint32_t n_cols = 0, 
    Color color = Color(0, 0, 0), 
    float height_step = 1.0f, 
    uint32_t n_terraces_on_height_step = 2);

  /// <summary>
  /// Добавить шестиугольник.
  /// </summary>
  /// <param name="row">
  /// Номер строки.
  /// </param>
  /// <param name="col">
  /// Номер столбца.
  /// </param>
  /// <param name="color">
  /// Цвет шестиугольника.
  /// </param>
  void add_hexagon(uint32_t row, uint32_t col, 
    Color color = Color(0, 0, 0));

  /// <summary>
  /// Создать случайную сетку.
  /// </summary>
  void generate_random_field();

  /// <summary>
  /// Вывести вершины и треугольники, на который треангулируется сетка.
  /// </summary>
  /// <param name="Vertices">
  /// Куда выводить вершины.
  /// </param>
  /// <param name="TriList">
  /// Куда выводить треугольники.
  /// </param>
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  /// <summary>
  /// Установить высоту шестиугольнику.
  /// </summary>
  /// <param name="row">
  /// Номер строки.
  /// </param>
  /// <param name="col">
  /// Номер столбца.
  /// </param>
  /// <param name="height">
  /// Новая высота.
  /// </param>
  void set_height(int row, int col, int32_t height);

  /// <summary>
  /// Установить цвет шестиугольнику.
  /// </summary>
  /// <param name="row">
  /// Номер строки.
  /// </param>
  /// <param name="col">
  /// Номер столбца.
  /// </param>
  /// <param name="color">
  /// Новый цвет.
  /// </param>
  void set_color(int row, int col, Color color);

private:
  /// <summary>
/// Базовая структура всех объектов
/// </summary>
  struct Object {
    virtual bool is_hexagon() { return false; }
    virtual bool is_triangle() { return false; }
    virtual bool is_rectangle() { return false; }
  };

  /// <summary>
  /// Структура координат
  /// </summary>
  struct Coord {
    /// <summary>
    /// Сравнение координат.
    /// </summary>
    /// <param name="rhs">
    /// Координаты для сравнения.
    /// </param>
    /// <returns>
    /// Результат сравнения
    /// </returns>
    bool operator< (const Coord& rhs) const
    {
      return (row < rhs.row) ||
        ((row == rhs.row) && (col < rhs.col));
    }
    uint32_t row, col;
  };

  /// <summary>
  /// Класс-синглтон, хранящий точки, которые требуются несколько раз.
  /// </summary>
  class Points {
  public:
    /// <summary>
    /// Получение экземпляра.
    /// </summary>
    /// <returns>
    /// Экземпляр.
    /// </returns>
    static Points& get_instance() {
      static Points instance;
      return instance;
    }

    /// <summary>
    /// Соединение точки с объектом.
    /// </summary>
    /// <param name="p">
    /// Точка.
    /// </param>
    /// <param name="object">
    /// Объект.
    /// </param>
    /// <returns>
    /// Id точки
    /// </returns>
    uint32_t connect_point_with_object(Point p,
      std::shared_ptr<Object> object);

    /// <summary>
    /// Проверка наличия точки.
    /// </summary>
    /// <param name="p">
    /// Точка.
    /// </param>
    /// <returns> </returns>
    bool in_points(Point p) const;

    /// <summary>
    /// Получение Id точки.
    /// </summary>
    /// <param name="p">
    /// Точка.
    /// </param>
    /// <returns>
    /// Id.
    /// </returns>
    uint32_t get_id_point(Point p);

    /// <summary>
    /// Получение объектов, связанных с точкой.
    /// </summary>
    /// <param name="id">
    /// Id точки.
    /// </param>
    /// <returns>
    /// Объекты.
    /// </returns>
    std::vector<std::shared_ptr<Object>> get_objects(uint32_t id);

    /// <summary>
    /// Получение точки по Id.
    /// </summary>
    /// <param name="id">
    /// Id.
    /// </param>
    /// <returns>
    /// Точка.
    /// </returns>
    Point get_point(uint32_t id) const;

    /// <summary>
    /// Присвоить Id новую точку.
    /// </summary>
    /// <param name="id">
    /// Id.
    /// </param>
    /// <param name="new_point">
    /// Новая точка.
    /// </param>
    void update_point(uint32_t id, Point new_point);

    /// <summary>
    /// Записать все точки в массив точек.
    /// </summary>
    /// <param name="Vertices">
    /// Массив точек.
    /// </param>
    void print_in_vertices(std::vector<PrintingPoint>& Vertices) {
      for (const Point& p : id_to_point)
        Vertices.push_back(p);
    }
  private:
    std::map<Point, uint32_t> point_to_id;
    std::vector<Point> id_to_point;
    std::vector<std::vector<std::shared_ptr<Object>>> id_to_objects;
  };

  /// <summary>
  /// Структура шестиугольник.
  /// </summary>
  struct Hexagon : Object {
    bool is_hexagon() { return true; }

    /// <summary>
    /// Создание пустого шестиугольника
    /// </summary>
    Hexagon() {}

    /// <summary>
    /// Создание шестиугольника.
    /// </summary>
    /// <param name="big_size">
    /// Радиус области покрытия.
    /// </param>
    /// <param name="small_size">
    /// Радиус отображаемого шестиугольника.
    /// </param>
    /// <param name="center">
    /// Центр шестиугольника.
    /// </param>
    /// <param name="pointDirection">
    /// Направление от центра на вершину.
    /// </param>
    /// <param name="floatDirection">
    /// Направление от центра на центр плоской грани.
    /// !!! должно быть перпендикулярно pointDirection !!!
    /// </param>
    /// <param name="color">
    /// Цвет шестиугольника.
    /// </param>
    Hexagon(float big_size, float small_size, Eigen::Vector3d center,
      Eigen::Vector3d pointDirection, Eigen::Vector3d floatDirection,
      Color color = Color(0, 0, 0));

    /// <summary>
    /// Соединение точек с множеством всех точек.
    /// </summary>
    /// <param name="ptr">
    /// Указатель на наш шестиугольник.
    /// </param>
    void connect_points(std::shared_ptr<Hexagon> ptr);

    /// <summary>
    /// Установить цвет шестиугольнику.
    /// </summary>
    /// <param name="color">
    /// Цвет.
    /// </param>
    void set_color(Color color);

    /// <summary>
    /// Вывести вершины и треугольники
    /// </summary>
    /// <param name="Vertices">
    /// Куда выводить вершины.
    /// </param>
    /// <param name="TriList">
    /// Куда выводить треугольники.
    /// </param>
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList) const;

    std::vector<uint32_t> innerPointsId, outerPointsId;
    Point center;
    std::vector<Point> innerPoints;
    std::vector<Point> outerPoints;
  };

  /// <summary>
  /// Класс треугольник.
  /// </summary>
  class Triangle : Object {
  public:
    bool is_triangle() { return true; }

    /// <summary>
    /// Создать треугольник.
    /// </summary>
    /// <param name="a">
    /// Точка a.
    /// </param>
    /// <param name="b">
    /// Точка b.
    /// </param>
    /// <param name="c">
    /// Точка с.
    /// </param>
    /// <param name="heightData">
    /// Данные для расчёта высот и террас.
    /// </param>
    Triangle(Point a, Point b, Point c);

    /// <summary>
    /// Создать треугольник.
    /// </summary>
    /// <param name="aId">
    /// Id точки a.
    /// </param>
    /// <param name="bId">
    /// Id точки b.
    /// </param>
    /// <param name="cId">
    /// Id точки c.
    /// </param>
    /// <param name="heightData">
    /// Данные для расчёта высот и террас.
    /// </param>
    Triangle(uint32_t aId, uint32_t bId, uint32_t cId);

    /// <summary>
    /// Оператор сравнения.
    /// Нужен для составления set и map треугольников.
    /// </summary>
    /// <param name="rhs">
    /// Треугольник для сравнения.
    /// </param>
    /// <returns>
    /// Результат сравнения.
    /// </returns>
    bool operator< (const Triangle& rhs) const;

    /// <summary>
    /// Вывести вершины и треугольники, на который треангулируется треугольник.
    /// </summary>
    /// <param name="Vertices">
    /// Куда выводить вершины.
    /// </param>
    /// <param name="TriList">
    /// Куда выводить треугольники.
    /// </param>
    /// </param name="hexagonGrid">
    /// Шестиугольная сетка к которой принадлежит треугольник.
    /// </param>
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList, 
      const HexagonGrid& hexagonGrid) const;

    uint32_t AId, BId, CId;
  private:
    /// <summary>
    /// Вывод террас.
    /// </summary>
    /// <param name="a">
    /// Точка a треугольника.
    /// </param>
    /// <param name="b">
    /// Точка b треугольника.
    /// </param>
    /// <param name="c">
    /// Точка c треугольника.
    /// </param>
    /// <param name="a_goal">
    /// Мнимая позиция точки a.
    /// </param>
    /// <param name="b_goal">
    /// Мнимая позиция точки b.
    /// </param>
    /// <param name="c_goal">
    /// Мнимая позиция точки c.
    /// </param>
    /// </param name="hexagonGrid">
    /// Шестиугольная сетка к которой принадлежит треугольник.
    /// </param>
    /// <param name="Vertices">
    /// Куда выводить вершины.
    /// </param>
    /// <param name="TriList">
    /// Куда выводить треугольники.
    /// </param>
    void print_stair(Point a, Point b, Point c,
      Eigen::Vector3d a_goal,
      Eigen::Vector3d b_goal,
      Eigen::Vector3d c_goal, 
      const HexagonGrid& hexagonGrid,
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList) const;
  };

  /// <summary>
  /// Класс прямоугольник между двух шестиугольников 
  /// </summary>
  class BorderRectangle : Object {
  public:
    bool is_rectangle() { return true; }

    /// <summary>
    /// Создание прямоугольника
    /// </summary>
    /// <param name="a">
    /// Точка a.
    /// </param>
    /// <param name="b">
    /// Точка b.
    /// </param>
    /// <param name="c">
    /// Точка c.
    /// </param>
    /// <param name="d">
    /// Точка d.
    /// </param>
    /// <param name="heightData">
    /// Данные для расчёта высот и террас.
    /// </param>
    BorderRectangle(Point a, Point b, Point c, Point d);

    /// <summary>
    /// Создание прямоугольника
    /// </summary>
    /// <param name="aId">
    /// Id точки a.
    /// </param>
    /// <param name="bId">
    /// Id точки b.
    /// </param>
    /// <param name="cId">
    /// Id точки c.
    /// </param>
    /// <param name="dId">
    /// Id точки d.
    /// </param>
    /// <param name="heightData">
    /// Данные для расчёта высот и террас.
    /// </param>
    BorderRectangle(uint32_t aId, uint32_t bId,
      uint32_t cId, uint32_t dId);

    /// <summary>
    /// Оператор сравнения.
    /// Нужен для составления set и map прямоугольников.
    /// </summary>
    /// <param name="rhs">
    /// Прямоугольник для сравнения.
    /// </param>
    /// <returns>
    /// Результат сравнения.
    /// </returns>
    bool operator< (const BorderRectangle& rhs) const;

    /// <summary>
    /// Вывести вершины и треугольники, на который треангулируется прямоугольник.
    /// </summary>
    /// <param name="Vertices">
    /// Куда выводить вершины.
    /// </param>
    /// <param name="TriList">
    /// Куда выводить треугольники.
    /// </param>
    void print_in_vertices_and_triList(
      std::vector<PrintingPoint>& Vertices,
      std::vector<uint16_t>& TriList,
      const HexagonGrid& hexagonGrid) const;

    uint32_t AId, BId, CId, DId;
  };

  static uint32_t connect(Point point, std::shared_ptr<HexagonGrid::Object> object);
  static void printRect(std::pair<Point, Point> a,
    std::pair<Point, Point> b,
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList);
  static void printTri(Point a, Point b, Point c,
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList);

  std::map<Coord, std::shared_ptr<Hexagon>> grid_;
  std::set<Triangle> triangles;
  std::set<BorderRectangle> rectangles;
  Eigen::Vector3d rowDirection_;
  Eigen::Vector3d colDirection_;
  Eigen::Vector3d heightDirection_;
  Eigen::Vector3d origin_;
  float size_, innerSize_;
  float heightStep_;
  uint32_t nTerracesOnHeightStep_;
};