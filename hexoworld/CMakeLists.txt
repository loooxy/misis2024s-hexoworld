add_library(hexoworld 
defines.hpp 
includes.hpp 
hexoworld.hpp 

manager/manager.hpp
manager/manager.cpp

base_objects/hexagon/hexagon.hpp
base_objects/hexagon/hexagon.cpp
base_objects/hexagon/drawers.cpp
base_objects/hexagon/frames.cpp

base_objects/triangle/triangle.hpp
base_objects/triangle/triangle.cpp
base_objects/triangle/drawers.cpp
base_objects/triangle/frames.cpp

base_objects/rectangle/rectangle.hpp
base_objects/rectangle/rectangle.cpp
base_objects/rectangle/drawers.cpp
base_objects/rectangle/frames.cpp

wall/wall.hpp
wall/wall.cpp
wall/drawers.cpp
wall/frames.cpp

cottage/cottage.hpp
cottage/cottage.cpp
cottage/drawers.cpp
cottage/frames.cpp

static_functions.cpp
static_variable.cpp
points.cpp
hexoworld.cpp)
target_link_libraries(hexoworld Eigen3::Eigen)
set_property(TARGET hexoworld PROPERTY CXX_STANDARD 20)
target_include_directories(hexoworld PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS hexoworld)