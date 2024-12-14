add_library(client 
client.hpp 
client.cpp
)

target_link_libraries(client 
cppzmq

frontend
clock
eventid
zmqhelper
)

set_property(TARGET client PROPERTY CXX_STANDARD 20)
target_include_directories(client PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS client)