add_library(render
render.hpp
render.cpp
)

target_link_libraries(render
glfw
Eigen3::Eigen
imgui::imgui
glad::glad
glm::glm
cereal::cereal
assimp::assimp

shader_s
camera
data_pool
hexoworld
workwithmap
events
mesh
model
stb_image
command
)

set_property(TARGET render PROPERTY CXX_STANDARD 20)
target_include_directories(render PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS render)