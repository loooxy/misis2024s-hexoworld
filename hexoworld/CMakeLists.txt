add_library(hexoworld 
hexoworld.hpp 

manager/manager.hpp
manager/manager.cpp

hexagon/hexagon.hpp
hexagon/hexagon.cpp
hexagon/drawers.cpp
hexagon/frames.cpp

triangle/triangle.hpp
triangle/triangle.cpp
triangle/drawers.cpp
triangle/frames.cpp

rectangle/rectangle.hpp
rectangle/rectangle.cpp
rectangle/drawers.cpp
rectangle/frames.cpp

wall/wall.hpp
wall/wall.cpp
wall/drawers.cpp
wall/frames.cpp

static_functions.cpp
points.cpp
hexoworld.cpp
drawers.cpp)
target_link_libraries(hexoworld Eigen3::Eigen)
set_property(TARGET hexoworld PROPERTY CXX_STANDARD 20)
target_include_directories(hexoworld PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS hexoworld)