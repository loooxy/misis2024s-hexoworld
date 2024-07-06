add_library(hexoworld hexoworld.cpp hexoworld.hpp)
set_property(TARGET hexoworld PROPERTY CXX_STANDARD 20)
target_include_directories(hexoworld PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS hexoworld)