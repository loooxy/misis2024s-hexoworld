add_library(data_pool 
data_pool.hpp
data_pool.cpp
)

target_link_libraries(data_pool 

hexoworld
)

set_property(TARGET data_pool PROPERTY CXX_STANDARD 20)
target_include_directories(data_pool PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS data_pool)