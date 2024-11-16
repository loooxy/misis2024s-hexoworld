add_library(server 
server.hpp 
server.cpp
)

target_link_libraries(server
glfw
Eigen3::Eigen
imgui::imgui
glad::glad
glm::glm
cppzmq

hexoworld
backend
)

set_property(TARGET server PROPERTY CXX_STANDARD 20)
target_include_directories(server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS server)