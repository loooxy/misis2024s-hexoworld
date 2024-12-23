add_library(mesh
mesh.hpp
mesh.cpp
)

target_link_libraries(mesh
glad::glad
glm::glm 
)

set_property(TARGET mesh PROPERTY CXX_STANDARD 20)
target_include_directories(mesh PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS mesh)