add_library(model
model.hpp
model.cpp
)

target_link_libraries(model
glad::glad
glm::glm 
assimp::assimp
stb_image
)

set_property(TARGET model PROPERTY CXX_STANDARD 20)
target_include_directories(model PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS model)