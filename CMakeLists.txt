cmake_minimum_required(VERSION 3.20)

project(misis2024s-hexoworld)


set(CMAKE_CXX_STANDARD 20)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(imgui REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(cppzmq REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}bin.relwithdbg)

set(EMSCRIPTEN_PTHREADS_COMPILER_FLAGS "-pthread")
set(EMSCRIPTEN_PTHREADS_COMPILER_FLAGS "-s PTHREAD_POOL_SIZE_STRICT=0")
set(EMSCRIPTEN_PTHREADS_LINKER_FLAGS "${EMSCRIPTEN_PTHREADS_COMPILER_FLAGS}") # -sPROXY_TO_PTHREAD
set(EMSCRIPTEN_PTHREADS_LINKER_FLAGS "-s PTHREAD_POOL_SIZE_STRICT=0")
#<script src="coi-serviceworker.js"></script>
if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(APPEND CMAKE_C_FLAGS " ${EMSCRIPTEN_PTHREADS_COMPILER_FLAGS}")
    string(APPEND CMAKE_CXX_FLAGS " ${EMSCRIPTEN_PTHREADS_COMPILER_FLAGS}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " ${EMSCRIPTEN_PTHREADS_LINKER_FLAGS}")
endif ()

#cmake -S . -B build -DWITH_PTHREADS=1 "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=/Users/kikikhakiri/emsdk/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
#"-DCMAKE_TOOLCHAIN_FILE=/Users/kikikhakiri/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=wasm32-emscripten"





add_subdirectory(opengl)
add_subdirectory(hexoworld)
add_subdirectory(application)

add_executable(map_render map_render.cpp)
target_link_libraries(map_render
hexoworld
application
)
if(EMSCRIPTEN)
    set_target_properties(map_render
            PROPERTIES
            LINK_FLAGS "${EMSCRIPTEN_PTHREADS_LINKER_FLAGS}"
    )
endif()

