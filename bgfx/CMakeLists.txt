find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}bin.relwithdbg)

add_executable(rotating_cube rotating_cube.cpp)
target_link_libraries(rotating_cube 
bgfx::bx 
bgfx::bgfx 
bgfx::bimg 
bgfx::bimg_decode 
glfw
)