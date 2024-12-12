add_library(workwithmap 
workwithmap.hpp
workwithmap.cpp
)

target_link_libraries(workwithmap
hexoworld
data_pool

cereal::cereal
)

set_property(TARGET workwithmap PROPERTY CXX_STANDARD 20)
target_include_directories(workwithmap PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS workwithmap)