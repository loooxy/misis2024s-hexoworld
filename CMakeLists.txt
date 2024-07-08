cmake_minimum_required(VERSION 3.20)

project(misis2024s-hexoworld)


set(CMAKE_CXX_STANDARD 20)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

find_package(bgfx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}bin.relwithdbg)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-16>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-16>")

add_subdirectory(hexoworld)

add_executable(map_render map_render.cpp)
target_link_libraries(map_render
bgfx::bx 
bgfx::bgfx 
bgfx::bimg 
bgfx::bimg_decode 
glfw
Eigen3::Eigen
hexoworld
)