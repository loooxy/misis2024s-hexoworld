add_library(bigg bigg.hpp bigg.cpp bigg_assets.h bigg_imgui.hpp bigg_shaders.hpp)
target_link_libraries(bigg bgfx::bx
        bgfx::bgfx
        bgfx::bimg
        bgfx::bimg_decode
        imgui::imgui
        glfw)
set_property(TARGET bigg PROPERTY CXX_STANDARD 20)
target_include_directories(bigg PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS bigg)