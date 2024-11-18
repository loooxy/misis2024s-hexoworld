add_library(events
events.hpp 
events.cpp
)

target_link_libraries(events 
cereal::cereal
workwithmap
)

set_property(TARGET events PROPERTY CXX_STANDARD 20)
target_include_directories(events PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS events)