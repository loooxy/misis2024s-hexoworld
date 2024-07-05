add_executable(hexagon_test hexagon_test.cpp)
target_link_libraries(hexagon_test hexagon)

find_package(libigl CONFIG REQUIRED)  

add_executable(vizualizator vizualizator.cpp)
target_link_libraries(vizualizator 
    PRIVATE 
        igl::igl_core 
        igl::igl_opengl
        igl::igl_glfw
)
