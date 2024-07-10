find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(hexagon_test hexagon_test.cpp)
target_link_libraries(hexagon_test hexagon)
add_executable(vizualizator vizualizator.cpp)
target_link_libraries(vizualizator hexagon 
bgfx::bx 
bgfx::bgfx 
bgfx::bimg 
bgfx::bimg_decode 
glfw)
add_executable(hexoworld_viewer_libigl hexoworld_viewer_libigl.cpp)
target_link_libraries(hexoworld_viewer_libigl 
igl::glfw
Eigen3::Eigen
hexoworld
)
