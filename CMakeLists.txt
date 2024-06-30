cmake_minimum_required(VERSION 3.20)

project(misis2024s-hexoworld)


set(CMAKE_CXX_STANDARD 20)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")



add_subdirectory(src)
add_subdirectory(tests)