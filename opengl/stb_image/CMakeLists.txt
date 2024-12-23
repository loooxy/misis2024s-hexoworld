add_library(stb_image
stb_image.h
stb_image.cpp
)

set_property(TARGET stb_image PROPERTY CXX_STANDARD 20)
target_include_directories(stb_image PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS stb_image)