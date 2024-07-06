find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(rotating_cube rotating_cube.cpp)
target_link_libraries(rotating_cube 
bgfx::bx 
bgfx::bgfx 
bgfx::bimg 
bgfx::bimg_decode 
glfw
)