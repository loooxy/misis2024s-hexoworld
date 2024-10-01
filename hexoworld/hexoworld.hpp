#pragma once
#include <thread>
#include <stdint.h>
#include <iomanip>
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <hexoworld/defines.hpp>

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

  /// \brief Оператор сравнения
  /// \param rhs С чем сравниваем.
  /// \return Результат сравнения.
  bool operator< (const PrintingPoint& rhs) const {
    float precision = 0.0001;
    if (x + precision < rhs.x)
      return true;
    if (x - precision > rhs.x)
      return false;

    if (y + precision < rhs.y)
      return true;
    if (y - precision > rhs.y)
      return false;

    if (z + precision < rhs.z)
      return true;
    if (z - precision > rhs.z)
      return false;

    if (abgr < rhs.abgr)
      return true;
    if (abgr > rhs.abgr)
      return false;

    return false;
  }

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
  void del_hexagon(uint32_t row, uint32_t col);

  /// \brief Добавить реку
  /// \param hexs Координаты {row, col} шестиугольников, по которым течёт река.
  void add_river(std::vector<std::pair<uint32_t, uint32_t>> hexs);

  /// \brief Затопить шестиугольник.
  /// \param row Строка позиции шестиугольника.
  /// \param col Столбец позиции шестиугольника.
  void add_flood_in_hex(uint32_t row, uint32_t col);
  void del_flood_in_hex(uint32_t row, uint32_t col);

  /// \brief Добавить дорогу в шестиугольнике.
  /// \param row Строка позиции шестиугольника.
  /// \param col Столбец позиции шестиугольника.
  void add_road_in_hex(uint32_t row, uint32_t col);
  void del_road_in_hex(uint32_t row, uint32_t col);

  /// \brief Создать ферму в шестиугольнике.
  /// \param row Строка позиции шестиугольника.
  /// \param col Столбец позиции шестиугольника.
  void add_farm_in_hex(uint32_t row, uint32_t col);
  void del_farm_in_hex(uint32_t row, uint32_t col);

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

  int32_t get_hex_height(uint32_t row, uint32_t col) const;
  Eigen::Vector4i get_hex_color(uint32_t row, uint32_t col) const;
  uint32_t get_n_rows() const;
  uint32_t get_n_cols() const;

  void show_numbers();
  void hide_numbers();

  /// \brief Вывести вершины и треугольники, на который треангулируется мир.
  /// \param Vertices Куда выводить вершины.
  /// \param TriList Куда выводить треугольники.
  void print_in_vertices_and_triList(
    std::vector<PrintingPoint>& Vertices,
    std::vector<uint16_t>& TriList) const;

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

  class IdType {
  public:
    explicit IdType()                   noexcept : real(false), id(0)      { }
    explicit IdType(const uint32_t id)  noexcept : real(true ), id(id)     { cnts[id]++; }
             IdType(const IdType& rhs)  noexcept : real(true ), id(rhs.id) { cnts[id]++; }
             IdType(const IdType&& rhs) noexcept : real(true ), id(rhs.id) { cnts[id]++; }
            ~IdType() {
               if (real)
               {
                 cnts[id]--;
                 if (cnts[id] == 0)
                 {
                   uint32_t div = id / Points::get_instance().colors_on_point;
                   uint32_t mod = id % Points::get_instance().colors_on_point;

                   if (div < Points::get_instance().points_id.size() &&
                     Points::get_instance().points_id[div].find(mod) != Points::get_instance().points_id[div].end())
                   {
                     Points::get_instance().id_to_essences[div].erase(
                       Points::get_instance().points_id[div][mod]);
                     Points::get_instance().points_id[div].erase(mod);
                   }
                 }
               }
             }

    IdType operator= (const IdType& rhs) {
      IdType::~IdType();

      id = rhs.id;
      real = rhs.real;
      cnts[id]++;
      return *this;
    }
    bool operator< (const IdType& rhs) const
    {
      return id < rhs.id;
    }

    uint32_t get() const { 
      if (real)
        return id;
      else
        throw std::runtime_error("You cant use not real id");
    }
  private:
    bool real = true;
    uint32_t id;
    static std::map<uint32_t, uint32_t> cnts;
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

  /// \brief Сущность от которой наследуется всё.
  class Essence {};

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
    void set_point_color(const IdType& point, Eigen::Vector4i color);

    /// \brief Получение цвета точки.
    /// \param point Точка.
    /// \param base Сущность, к которой относится точка.
    /// \return Цвет точки.
    Eigen::Vector4i get_point_color(Eigen::Vector3d point, const Essence* base);

    /// \brief Проверка наличия точки.
    /// \param p Точка.
    /// \return true - если точка есть, иначе false
    bool in_points(Eigen::Vector3d p) const;

    /// \brief Получение Id точки.
    /// \param p Точка.
    /// \param base Объект, к которому принадлежит точка.
    /// \return Id.
    IdType get_id_point(Eigen::Vector3d p, const Essence* base);

    /// \brief Получение точки по Id.
    /// \param id Id.
    /// \return Точка.
    Eigen::Vector3d get_point(const IdType& id) const;

    /// \brief Присвоить Id новую точку.
    /// \param id Id.
    /// \param new_point Новая точка.
    void update_point(const IdType& id, Eigen::Vector3d new_point);

    /// \brief Записать все точки в массив точек.
    /// \param Vertices Массив точек.
    void print_in_vertices(std::vector<PrintingPoint>& Vertices);

    void lock() { is_locked = true; }
    void unlock() { is_locked = false; }
  private:
    std::map<Eigen::Vector3d, uint32_t, EigenVector3dComp> point_to_id; //< Id точек.
    std::vector<Eigen::Vector3d> id_to_point; /*< 
    массив точек.На i - той позиции стоит точка,
    Id которой с colors_on_point * i по colors_on_point * (i + 1) - 1.*/
    std::vector<std::vector<Eigen::Vector4i>> id_to_color;//< Цвета точек.
    std::vector<std::map<const Essence*, uint32_t>> id_to_essences; //< Объекты точек.
    std::vector<std::map<uint32_t, const Essence*>> points_id;
    const uint32_t colors_on_point = 5; //< Максимальное число цветов на точку
    bool is_locked = false;
    friend IdType;
    mutable std::recursive_timed_mutex points_mtx;
  };

  /// \brief Класс отрисовщиков.
  template<class BaseType> //< Тип базового объекта.
  class Drawer : public Essence {
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
  class Frame : public Essence {
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому принадлежит каркас.
    Frame(BaseType* base) : base(base) {}

    /// \brief Получить все точки объекта.
    /// \return Масси точек.
    virtual std::vector<Eigen::Vector3d> get_points() const = 0;

    /// \brief Вывести треугольники, на который треангулируется объект.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint32_t>& TriList) const = 0;
  protected:
    BaseType* base; //< Объект, к которому принадлежит каркас.
  };

  /// \brief Базовая структура всех объектов
  struct Object;

  /// \brief Базовый класс неподвижного инвентаря.
  class FixedInventory : public Essence{
  public:
    /// \brief Конструктор.
    /// \param base Объект, к которому относится инвентарь.
    FixedInventory(Object* base) : base(base){}

    /// \brief Получить все точки инвентаря.
    /// \return Масси точек.
    virtual std::vector<Eigen::Vector3d> get_points() const = 0;

    /// \brief Вывести треугольники, на который треангулируется инвентарь.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint32_t>& TriList) const = 0;
    
    /// \brief Раскрашивание точек.
    virtual void colorize_points() = 0;

    std::map<uint32_t, std::shared_ptr<Drawer<FixedInventory>>> drawers; //< Отрисовщики.
    std::map<uint32_t, std::shared_ptr<Frame<FixedInventory>>> frames;   //< Каркасы.
    Object* base; //< Объект, к которому отнисится инвентарь.
  };

  enum FrameAndDrawersTypes
    {
      Usual, //< Обычные, обязательные, каркасы и отрисовщики.
      River, //< Каркасы и отрисовщики рек.
      Road,  //< Каркасы и отрисовщики дорог.
      Flood  //< Каркасы и отрисовщики стоящей воды.
    };

  /// \brief Базовая структура всех объектов
  struct Object : public Essence{
    /// \brief Конструктор по умолчанию.
    /// \param world Мир, к которому принадлежит объект.
    Object(Hexoworld& world) :
      world(world) {}

    /// \brief Вывести треугольники, на который треангулируется объект.
    /// \param TriList Куда выводить треугольники.
    virtual void print_in_triList(std::vector<uint32_t>& TriList) = 0;

    /// \brief Раскрашивание точек.
    virtual void colorize_points() = 0;
    
    std::map<FrameAndDrawersTypes, std::shared_ptr<Drawer<Object>>> drawers; //< Отрисовщики.
    std::map<FrameAndDrawersTypes, std::shared_ptr<Frame<Object>>> frames;   //< Каркасы.
    std::vector<std::shared_ptr<FixedInventory>> inventory; //< Неподвижный инвентарь на объекте.
    Hexoworld& world; ///< Мир к которому принадлежит шестиугольник.
  };

  

  /// \brief Структура шестиугольник.
  struct Hexagon;

  /// \brief Класс треугольник.
  class Triangle;

  /// \brief Класс прямоугольник между двух шестиугольников.
  class Rectangle;

  /// \brief Стена.
  class Wall;

  /// \brief Частный дом.
  class Cottage;

  /// \brief Класс менеджера.
  class Manager;

  /// \brief Установить высоту точке.
  /// \param point Точка.
  /// \param height Новая высота.
  /// \param type К какому каркасу/отрисовщику относится.
  void set_new_height_to_point(Eigen::Vector3d& point, int32_t height, FrameAndDrawersTypes type);
  
  /// \brief Установить высоту точке.
  /// \param id_point Id точки.
  /// \param height Новая высота.
  /// \param type К какому каркасу/отрисовщику относится.
  Eigen::Vector3d set_new_height_to_point(IdType id_point, int32_t height, FrameAndDrawersTypes type);

  /// \brief Получить номер грани между двумя вершинами.
  /// \param vertex1 Первая вершина.
  /// \param vertex2 Вторая вершина.
  /// \return Номер грани.
  static uint32_t get_ind_extraPoints(uint32_t vertex1, uint32_t vertex2);

  /// \brief Вывод прямоугольника.
  /// \param aIds Id первой пары точек.
  /// \param bIds Id второй пары точек.
  /// a.first-b.first должно быть параллельно a.second-b.second
  /// \param TriList Куда выводить треугольники.
  static void printRect(const std::pair<IdType, IdType>& aIds,
    const std::pair<IdType, IdType>& bIds,
    std::vector<uint32_t>& TriList);

  /// \brief Вывод треугольника.
  /// \param aId Id точки a.
  /// \param bId Id точки b.
  /// \param cId Id точки c.
  /// \param TriList Куда выводить треугольники.
  static void printTri(const IdType& aId, const IdType& bId, const IdType& cId,
    std::vector<uint32_t>& TriList);

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
    return type * 10 * PRECISION_DBL_CALC * heightDirection_;
  }

  static void zip_data(std::vector<PrintingPoint>& Vertices,
    std::vector<uint32_t>& TriList,
    std::vector<PrintingPoint>& new_Vertices,
    std::vector<uint16_t>& new_TriList);

  const Eigen::Vector3d rowDirection_; ///< Направление увеличения номера строки.
  const Eigen::Vector3d colDirection_; ///< Направление увеличения номера столбца.
  const Eigen::Vector3d heightDirection_;///< Направление вверх.
  const Eigen::Vector3d origin_; ///< Начало координат.
  const float size_; ///< Радиус внутреннего шестиугольника.
  const float heightStep_; ///< Расстояние между соседними уровнями. 
  const uint32_t nTerracesOnHeightStep_;///< Число террас между соседними уровнями.
  const uint32_t n_rows; ///< Кол-во рядов
  const uint32_t n_cols; ///< Кол-во столбцоы

  Eigen::Vector4i riverColor = Eigen::Vector4i(0, 100, 255, 255); //< Цвет воды в реке.
  Eigen::Vector4i floodColor = Eigen::Vector4i(72, 209, 204, 1); //< Цвет затопления.
  Eigen::Vector4i roadColor = Eigen::Vector4i(96, 96, 96, 255); //< Цвет дороги.

  std::unique_ptr<Manager> manager; ///< Менеджер.
  mutable std::recursive_timed_mutex main_mtx;

  static const double PRECISION_DBL_CALC; ///< Точность вещественных вычислений.
};