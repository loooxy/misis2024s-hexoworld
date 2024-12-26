add_library(application 
application.hpp 
application.cpp
)

target_link_libraries(application 
server
client

cppzmq
)

set_property(TARGET application PROPERTY CXX_STANDARD 20)
target_include_directories(application PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS application)

add_subdirectory(events)
add_subdirectory(data_pool)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(workwithmap)
add_subdirectory(frontend)
add_subdirectory(events_queue)
add_subdirectory(backend)
add_subdirectory(command)
add_subdirectory(render)
add_subdirectory(clock)
add_subdirectory(eventid)
add_subdirectory(zmqhelper)