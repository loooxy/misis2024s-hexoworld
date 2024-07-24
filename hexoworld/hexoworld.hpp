#pragma once
#include <stdint.h>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>

/// \brief Структура формата вывода точки.
struct PrintingPoint {
  /// \brief Конструктор по умолчанию
  PrintingPoint() = default;
  /// \brief Конструктор по позиции и цвету.
  /// \param position Позиция точки.
  /// \param color Цвет точки.
  PrintingPoint(Eigen::Vector3d position, 
    Eigen::Vector4i color) 
    : x(position.x()), y(position.y()), z(position.z()) {
    abgr = (
      (color.w() % 256) << 24 | 
      (color.z() % 256) << 16 |
      (color.y() % 256) << 8  |
      (color.x() % 256)
      );
  };
  float x, y, z; //< Позиция.
  uint32_t abgr; //< Цвет.
};

/// \brief Класс шестиугольного мира.
class Hexoworld
{
public:
  /// \brief Конструктор по умолчанию запрещён, так как необходимы параметры мира.
  Hexoworld() = delete;

  /// \brief Создает шестиугольный мир.
  /// \param size Радиус шестиугольников.
  /// \param origin Начало координат.
  /// \param row_direction Направление увеличения номера строки.
  /// \param сol_direction Направление увеличения номера столбца.
  /// !!! должно быть перпендикулярно row_direction !!!
  /// \param height_step Высота между соседними уровнями.
  /// \param n_terraces_on_height_step Количество террас между соседними уровнями.
  /// \param n_rows Количество строк.
  /// \param n_cols Количество столбцов.
  Hexoworld(float size, Eigen::Vector3d origin,
    Eigen::Vector3d row_direction, Eigen::Vector3d сol_direction,
    float height_step = 1.0f,
    uint32_t n_terraces_on_height_step = 2,
    uint32_t n_rows = 0, uint32_t n_cols = 0);

  /// \brief Добавить шестиугольник.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param color Цвет шестиугольника.
  void add_hexagon(uint32_t row, uint32_t col, Eigen::Vector4i color);

  /// \brief Добавить реку
  /// \param hexs Координаты {row, col} шестиугольников, по которым течёт река.
  void add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs);

  /// \brief Затопить шестиугольник.
  /// \param row Строка позиции шестиугольника.
  /// \param col Столбец позиции шестиугольника.
  void flood_hex(uint32_t row, uint32_t col);

  /// \brief Добавить дорогу в шестиугольнике.
  /// \param row Строка позиции шестиугольника.
  /// \param col Столбец позиции шестиугольника.
  void add_road_in_hex(uint32_t row, uint32_t col);

  /// \brief Обновить реку.
  void update_river();

  /// \brief Установить высоту шестиугольнику.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param height Новая высота.
  void set_hex_height(uint32_t row, uint32_t col, int32_t height);

  /// \brief Установить цвет шестиугольнику.
  /// \param row Номер строки.
  /// \param col Номер столбца.
  /// \param color Новый цвет шестиугольника.
  void set_hex_color(uint32_t row, uint32_t col, Eigen::Vector4i color);

  /// \brief Вывести вершины и треугольники, на который треангулируется мир.
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

  /// \brief Присвоить вершинам новый цвет.
  /// \param Vertices Вершины.
  void recolor(std::vector<PrintingPoint>& Vertices) const;
private:
  /// \brief Класс-компаратор для Eigen::Vector3d
  class EigenVector3dComp {
  public:
    EigenVector3dComp() = default;
    bool operator()(const Eigen::Vector3d& a,
      const Eigen::Vector3d& b) const {
      if (a.x() + PRECISION_DBL_CALC < b.x())
        return true;
      if (b.x() + PRECISION_DBL_CALC < a.x())
        return false;

      if (a.y() + PRECISION_DBL_CALC < b.y())
        return true;
      if (b.y() + PRECISION_DBL_CALC < a.y())
        return false;

      if (a.z() + PRECISION_DBL_CALC < b.z())
        return true;

      return false;
    }
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

  /// \brief Базовая структура всех объектов
  struct Object;

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

    /// \brief Установить цвет точке.
    /// \param point Точка.
    /// \param color Цвет.
    /// \param base Объект, к которому принадлежит точка.
    void set_point_color(Eigen::Vector3d point, Eigen::Vector4i color, const Object* base);

    /// \brief Проверка наличия точки.
    /// \param p Точка.
    /// \return true - если точка есть, иначе false
    bool in_points(Eigen::Vector3d p) const;

    /// \brief Получение Id точки.
    /// \param p Точка.
    /// \param base Объект, к которому принадлежит точка.
    /// \return Id.
    uint32_t get_id_point(Eigen::Vector3d p, const Object* base);

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
    void print_in_vertices(std::vector<PrintingPoint>& Vertices);

    /// \brief Присвоить вершинам новый цвет.
    /// \param Vertices Вершины.
    void recolor(std::vector<PrintingPoint>& Vertices);

  private:
    std::map<Eigen::Vector3d, uint32_t, EigenVector3dComp> point_to_id; //< Id точек.
    std::vector<Eigen::Vector3d> id_to_point; 
    //< массив точек. На i-той позиции стоит точка, 
    //Id которой с colors_on_point * i по colors_on_point * (i + 1) - 1.
    std::vector<std::vector<Eigen::Vector4i>> id_to_color;//< Цвета точек.
    std::vector<std::map<const Object*, uint32_t>> id_to_objects; //< Объекты точек.
    const uint32_t colors_on_point = 4; //< Максимальное число цветов на точку
  };

  /// \brief Класс отрисовщиков.
  template<class BaseType> //< Тип базового объекта.
  class Drawer {
  public:
    /// \brief Конструктор.
    /// \param object Объект, к которому относится отрисовщик.
    Drawer(BaseType* object) : base(object) {}

    /// \brief Раскрасить точки.
    virtual void colorize_points() = 0;
  protected:
    BaseType* base; //< Объект, к которому относится отрисовщик.
  };

  /// \brief Класс каркасов.
  template<class BaseType> //< Тип базового объекта.
  class Frame {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    Frame(BaseType* base) : base(base) {}

    /// \brief Получить все точки объекта.
    /// \return Масси точек.
    virtual std::vector<Eigen::Vector3d> get_points() const = 0;

    /// \brief Вывести треугольники, на который треангулируется объект.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint16_t>& TriList) const = 0;
  protected:
    BaseType* base; //< Объект, к которому принадлежит каркас.
  };

  /// \brief Базовый класс неподвижного инвентаря.
  class FixedInventory {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому относится инвентарь.
    FixedInventory(Object* base) : base(base){}

    /// \brief Получить все точки инвентаря.
    /// \return Масси точек.
    virtual std::vector<Eigen::Vector3d> get_points() const = 0;

    /// \brief Вывести треугольники, на который треангулируется инвентарь.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint16_t>& TriList) const = 0;

    std::map<uint32_t, std::shared_ptr<Drawer<FixedInventory>>> drawers; //< Отрисовщики.
    std::map<uint32_t, std::shared_ptr<Frame<FixedInventory>>> frames;   //< Каркасы.
    Object* base; //< Объект, к которому отнисится инвентарь.
  };

  /// \brief Базовая структура всех объектов
  struct Object {
    /// \brief Конструктор по умолчанию.
    /// \param world Мир, к которому принадлежит объект.
    Object(Hexoworld& world) :
      world(world) {}

    /// \brief Вывести треугольники, на который треангулируется объект.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint16_t>& TriList) = 0;
    
    std::map<uint32_t, std::shared_ptr<Drawer<Object>>> drawers; //< Отрисовщики.
    std::map<uint32_t, std::shared_ptr<Frame<Object>>> frames;   //< Каркасы.
    std::vector<std::shared_ptr<FixedInventory>> inventory; //< Неподвижный инвентарь на объекте.
    Hexoworld& world; ///< Мир к которому принадлежит шестиугольник.
  };

  enum FrameAndDrawersTypes
  {
    Usual, //< Обычные, обязательные, каркасы и отрисовщики.
    River, //< Каркасы и отрисовщики рек.
    Road,  //< Каркасы и отрисовщики дорог.
    Flood  //< Каркасы и отрисовщики стоящей воды.
  };

  /// \brief Структура шестиугольник.
  struct Hexagon;

  /// \brief Класс треугольник.
  class Triangle;

  /// \brief Класс прямоугольник между двух шестиугольников.
  class Rectangle;

  /// \brief Стена.
  class Wall;

  /// \brief Класс менеджера.
  class Manager;

  /// \brief Установить высоту точке.
  /// \param point Точка.
  /// \param height Новая высота.
  /// \param type К какому каркасу/отрисовщику относится.
  void set_new_height_to_point(Eigen::Vector3d& point, int32_t height, FrameAndDrawersTypes type);

  /// \brief Получить номер грани между двумя вершинами.
  /// \param vertex1 Первая вершина.
  /// \param vertex2 Вторая вершина.
  /// \return Номер грани.
  static uint32_t get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2);

  /// \brief Вывод прямоугольника.
  /// \param a Первая пара точек.
  /// \param b Вторая пара точек.
  /// a.first-b.first должно быть параллельно a.second-b.second
  /// \param TriList Куда выводить треугольники.
  /// \param base Объект, к которому принадлежат точки.
  static void printRect(std::pair<Eigen::Vector3d, Eigen::Vector3d> a,
    std::pair<Eigen::Vector3d, Eigen::Vector3d> b,
    std::vector<uint16_t>& TriList, const Object* base);

  /// \brief Вывод прямоугольника.
  /// \param aIds Id первой пары точек.
  /// \param bIds Id второй пары точек.
  /// a.first-b.first должно быть параллельно a.second-b.second
  /// \param TriList Куда выводить треугольники.
  static void printRect(std::pair<uint32_t, uint32_t> aIds,
    std::pair<uint32_t, uint32_t> bIds,
    std::vector<uint16_t>& TriList);

  /// \brief Вывод треугольника.
  /// \param a Точка a.
  /// \param b Точка b.
  /// \param c Точка c.
  /// \param TriList Куда выводить треугольники.
  /// \param base Объект, к которому принадлежат точки.
  static void printTri(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d c,
    std::vector<uint16_t>& TriList, const Object* base);

  /// \brief Вывод треугольника.
  /// \param aId Id точки a.
  /// \param bId Id точки b.
  /// \param cId Id точки c.
  /// \param TriList Куда выводить треугольники.
  static void printTri(uint32_t aId, uint32_t bId, uint32_t cId,
    std::vector<uint16_t>& TriList);

  /// \brief Получить номер грани, ведущей от шестиугольника с координатами a, 
  /// к шестиугольнику с координатами b.
  /// \param a Координата a.
  /// \param b Координата b.
  /// \return Номер грани.
  static uint32_t get_ind_direction(Hexoworld::Coord a, Hexoworld::Coord b);

  /// \brief Получить отсортированную пару координат.
  /// \param a Координата a.
  /// \param b Координата b.
  /// \return Отсортированная пара координат.
  static std::pair<Coord, Coord> pair_coords(Coord a, Coord b);

  /// \brief Получить отсортированную тройку координат.
  /// \param a Координата a.
  /// \param b Координата b.
  /// \param c Координата c.
  /// \return Отсортированная тройка координат.
  static std::vector<Coord> tri_coords(Coord a, Coord b, Coord c);

  /// \brief Получить дополнительные вектор вверх по типу каркаса/отрисовщика.
  /// \param type Тип каркаса/отрисовщика.
  /// \return Дополнительные вектор вверх.
  Eigen::Vector3d dop_height(FrameAndDrawersTypes type) {
    return type * 10000 * PRECISION_DBL_CALC * heightDirection_;
  }

  Eigen::Vector3d rowDirection_; ///< Направление увеличения номера строки.
  Eigen::Vector3d colDirection_; ///< Направление увеличения номера столбца.
  Eigen::Vector3d heightDirection_;///< Направление вверх.
  Eigen::Vector3d origin_; ///< Начало координат.
  float size_; ///< Радиус внутреннего шестиугольника.
  float heightStep_; ///< Расстояние между соседними уровнями. 
  uint32_t nTerracesOnHeightStep_;///< Число террас между соседними уровнями.

  Eigen::Vector4i riverColor = Eigen::Vector4i(0, 100, 255, 255); //< Цвет воды в реке.
  Eigen::Vector4i floodColor = Eigen::Vector4i(72, 209, 204, 1); //< Цвет затопления.
  Eigen::Vector4i roadColor = Eigen::Vector4i(96, 96, 96, 255); //< Цвет дороги.

  std::unique_ptr<Manager> manager; ///< Менеджер.
  static const double PRECISION_DBL_CALC; ///< Точность вещественных вычислений.
};

#include <hexoworld/hexagon/hexagon.hpp>
#include <hexoworld/rectangle/rectangle.hpp>
#include <hexoworld/triangle/triangle.hpp>
#include <hexoworld/manager/manager.hpp>
#include <hexoworld/wall/wall.hpp>