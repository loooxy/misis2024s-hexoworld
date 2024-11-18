add_library(backend
backend.hpp
backend.cpp
)

target_link_libraries(backend
cereal::cereal

workwithmap
data_pool
events_queue
events
)

set_property(TARGET backend PROPERTY CXX_STANDARD 20)
target_include_directories(backend PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS backend)