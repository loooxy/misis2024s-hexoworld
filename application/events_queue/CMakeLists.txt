add_library(events_queue
events_queue.hpp 
events_queue.cpp
)

set_property(TARGET events_queue PROPERTY CXX_STANDARD 20)
target_include_directories(events_queue PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS events_queue)