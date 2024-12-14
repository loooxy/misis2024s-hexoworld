add_library(server 
server.hpp 
server.cpp
)

target_link_libraries(server
cppzmq

backend
camera
clock
eventid
zmqhelper
)

set_property(TARGET server PROPERTY CXX_STANDARD 20)
target_include_directories(server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS server)