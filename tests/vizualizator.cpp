#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <bx/math.h>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "src/hexagon.cpp"

extern "C" {
    void* glfwGetCocoaWindow(GLFWwindow* window);
}

const char* vs_shader = R"(
#version 330 core
layout(location = 0) in vec3 a_position;
uniform mat4 u_modelViewProj;
void main() {
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
)";

// Фрагментный шейдер (GLSL)
const char* fs_shader = R"(
#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Красный цвет
}
)";

void glfw_errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void initBGFX(GLFWwindow* window) {
    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width = 1280;
    init.resolution.height = 720;
    init.resolution.reset = BGFX_RESET_VSYNC;

    bgfx::PlatformData pd;
#if defined(__APPLE__)
    pd.nwh = glfwGetCocoaWindow(window);
#elif defined(_WIN32)
    pd.nwh = glfwGetWin32Window(window);
#elif defined(__linux__)
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#endif
    init.platformData = pd;

    bgfx::init(init);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
}

static bgfx::VertexLayout PosVertexLayout;

void initVertexLayout() {
    PosVertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
}

struct PosVertex {
    float x, y, z;

    static void init() {
        PosVertexLayout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
    }
};

bgfx::ShaderHandle compileShader(bgfx::RendererType::Enum type, const char* source, const char* name) {
    const bgfx::Memory* mem = bgfx::copy(source, strlen(source) + 1);
    return bgfx::createShader(mem);
}

bgfx::ProgramHandle createProgram() {
    bgfx::ShaderHandle vs = compileShader(bgfx::RendererType::Enum::OpenGL, vs_shader, "vs_shader");
    bgfx::ShaderHandle fs = compileShader(bgfx::RendererType::Enum::OpenGL, fs_shader, "fs_shader");
    return bgfx::createProgram(vs, fs, true);
}

void renderHexagon(const Layout& layout, const Hex& hex, bgfx::ProgramHandle program) {
    std::vector<Point> corners = polygon_corners(layout, hex);
    std::vector<PosVertex> vertices;
    for (const auto& corner : corners) {
        vertices.push_back({float(corner.x), float(corner.y), 0.0f});
    }

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, vertices.size(), PosVertexLayout);

    memcpy(tvb.data, vertices.data(), vertices.size() * sizeof(PosVertex));

    const uint16_t indices[] = {0, 1, 2, 2, 3, 0, 3, 4, 5, 5, 0, 3};
    bgfx::TransientIndexBuffer tib;
    bgfx::allocTransientIndexBuffer(&tib, sizeof(indices) / sizeof(indices[0]));

    memcpy(tib.data, indices, sizeof(indices));

    float mtx[16];
    bx::mtxIdentity(mtx);
    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setIndexBuffer(&tib);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, program);
}

int main() {
    glfwSetErrorCallback(glfw_errorCallback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hexagon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    initBGFX(window);
    initVertexLayout();
    PosVertex::init();

    bgfx::ProgramHandle program = createProgram();

    Orientation orientation = Orientation(
        3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0),
        2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0,
        0.0
    );
    Layout layout = Layout(orientation, Point(10.0, 10.0), Point(0.0, 0.0));

    while (!glfwWindowShouldClose(window)) {
        bgfx::touch(0); // Устанавливает кадр для очистки

        for (int q = -2; q <= 2; ++q) {
            for (int r = -2; r <= 2; ++r) {
                int s = -q - r;
                if (abs(q) <= 2 && abs(r) <= 2 && abs(s) <= 2) {
                    Hex hex = Hex(q, r, s);
                    renderHexagon(layout, hex, program);
                }
            }
        }

        bgfx::frame();
        glfwPollEvents();
    }

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
