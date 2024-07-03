#include <stdio.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

// Simple vertex shader
static const char* vs_shader = R"(
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    }
)";

// Simple fragment shader
static const char* fs_shader = R"(
    void main() {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

static bool s_showStats = false;

static void glfw_errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

bgfx::ShaderHandle loadShader(const char* shader)
{
    const bgfx::Memory* mem = bgfx::copy(shader, (uint32_t)strlen(shader) + 1);
    return bgfx::createShader(mem);
}

bgfx::ProgramHandle loadProgram(const char* vs_name, const char* fs_name)
{
    bgfx::ShaderHandle vsh = loadShader(vs_name);
    bgfx::ShaderHandle fsh = loadShader(fs_name);
    return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true, true)
            .end();
    }

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

int main(int argc, char **argv)
{
    glfwSetErrorCallback(glfw_errorCallback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1024, 768, "helloworld", nullptr, nullptr);
    if (!window)
        return 1;
    glfwSetKeyCallback(window, glfw_keyCallback);
    bgfx::renderFrame();
    bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    init.platformData.ndt = glfwGetX11Display();
    init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
    init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
    init.platformData.nwh = glfwGetWin32Window(window);
#endif
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    init.resolution.width = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
        return 1;
    const bgfx::ViewId kClearView = 0;
    bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

    bgfx::ProgramHandle program = loadProgram(vs_shader, fs_shader);

    static PosColorVertex s_vertexData[7] =
    {
        {  0.0f,  0.0f, 0.0f, 0xff0000ff },
        { -0.5f,  0.5f, 0.0f, 0xff00ff00 },
        {  0.5f,  0.5f, 0.0f, 0xff00ff00 },
        {  1.0f,  0.0f, 0.0f, 0xff00ff00 },
        {  0.5f, -0.5f, 0.0f, 0xff00ff00 },
        { -0.5f, -0.5f, 0.0f, 0xff00ff00 },
        { -1.0f,  0.0f, 0.0f, 0xff00ff00 },
    };

    static const uint16_t s_indexData[18] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1,
    };

    PosColorVertex::init();

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(s_vertexData, sizeof(s_vertexData)),
        PosColorVertex::ms_layout
    );

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(s_indexData, sizeof(s_indexData))
    );

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int oldWidth = width, oldHeight = height;
        glfwGetWindowSize(window, &width, &height);
        if (width != oldWidth || height != oldHeight) {
            bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
            bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
        }
        bgfx::touch(kClearView);
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
        // Enable stats or debug text.
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::submit(kClearView, program);

        bgfx::frame();
    }
    bgfx::shutdown();
    glfwTerminate();
    return 0;
}