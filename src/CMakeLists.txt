add_library(hexagon hexagon.cpp)
set_property(TARGET hexagon PROPERTY CXX_STANDARD 20)
target_include_directories(hexagon PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS hexagon)