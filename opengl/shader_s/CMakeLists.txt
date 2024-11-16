add_library(shader_s
shader_s.hpp
shader_s.cpp
)

target_link_libraries(shader_s
glad::glad
glm::glm
)

set_property(TARGET shader_s PROPERTY CXX_STANDARD 20)
target_include_directories(shader_s PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS shader_s)