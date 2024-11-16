add_library(frontend 
frontend.hpp
frontend.cpp
)

target_link_libraries(frontend
glfw
Eigen3::Eigen
imgui::imgui
glad::glad
glm::glm
cppzmq
cereal

data_pool
hexoworld
)

set_property(TARGET frontend PROPERTY CXX_STANDARD 20)
target_include_directories(frontend PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS frontend)