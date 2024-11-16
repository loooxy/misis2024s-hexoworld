add_library(camera
camera.hpp
camera.cpp
)

target_link_libraries(camera
glad::glad
glm::glm
)

set_property(TARGET camera PROPERTY CXX_STANDARD 20)
target_include_directories(camera PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS camera)