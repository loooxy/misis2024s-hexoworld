add_library(zmqhelper
zmqhelper.hpp
zmqhelper.cpp
)

target_link_libraries(zmqhelper cppzmq)

set_property(TARGET zmqhelper PROPERTY CXX_STANDARD 20)
target_include_directories(zmqhelper PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS zmqhelper)