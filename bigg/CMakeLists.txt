add_library(bigg bigg.hpp bigg_assets.h bigg_imgui.hpp bigg_shaders.hpp bigg.cpp)
target_link_libraries(bigg imgui::imgui glfw glm::glm
        bgfx::bx
        bgfx::bgfx
        bgfx::bimg
        bgfx::bimg_decode )
set_property(TARGET bigg PROPERTY CXX_STANDARD 20)
target_include_directories(bigg PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS bigg)