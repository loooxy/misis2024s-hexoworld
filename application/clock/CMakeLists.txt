add_library(clock
clock.hpp
clock.cpp
)

set_property(TARGET clock PROPERTY CXX_STANDARD 20)
target_include_directories(clock PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS clock)