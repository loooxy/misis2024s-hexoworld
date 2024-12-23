add_library(command
command.hpp 
command.cpp
)

target_link_libraries(command
cereal::cereal

camera)

set_property(TARGET command PROPERTY CXX_STANDARD 20)
target_include_directories(command PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS command)