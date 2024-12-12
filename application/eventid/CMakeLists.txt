add_library(eventid
eventid.hpp
eventid.cpp
)

target_link_libraries(eventid cereal::cereal)

set_property(TARGET eventid PROPERTY CXX_STANDARD 20)
target_include_directories(eventid PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS eventid)