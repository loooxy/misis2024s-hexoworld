add_library(application 
application.hpp 
application.cpp
workwithmap.cpp
frontend.cpp)

target_link_libraries(application 
bgfx::bx 
bgfx::bgfx 
bgfx::bimg 
bgfx::bimg_decode 
glfw
Eigen3::Eigen
imgui::imgui
glad::glad
glm::glm

hexoworld
)

set_property(TARGET application PROPERTY CXX_STANDARD 20)
target_include_directories(application PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS application)