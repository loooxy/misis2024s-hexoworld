find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(hexoworld hexoworld.hpp hexoworld.cpp)
target_link_libraries(hexoworld Eigen3::Eigen)
set_property(TARGET hexoworld PROPERTY CXX_STANDARD 20)
target_include_directories(hexoworld PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS hexoworld)