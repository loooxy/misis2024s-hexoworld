#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#define Debug(x) std::cout << #x << " = " << x << std::endl;
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "GLFW/glfw3.h"
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "GLFW/glfw3native.h"

#include<hexoworld/hexoworld.hpp>

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

bgfx::ShaderHandle loadShader(std::string filename)
{
  std::string path = "../shaders/" + filename;
  FILE* file = fopen(path.c_str(), "rb");
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
  fread(mem->data, 1, fileSize, file);
  mem->data[mem->size - 1] = '\0';
  fclose(file);

  return bgfx::createShader(mem);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "Hello, bgfx!", NULL, NULL);
  glfwSetKeyCallback(window, key_callback);

  bgfx::renderFrame();

  bgfx::Init bgfxInit;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  bgfxInit.platformData.ndt = glfwGetX11Display();
  bgfxInit.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
  bgfxInit.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
  bgfxInit.platformData.nwh = glfwGetWin32Window(window);
#endif

  bgfxInit.type = bgfx::RendererType::Count;
  bgfxInit.resolution.width = WNDW_WIDTH;
  bgfxInit.resolution.height = WNDW_HEIGHT;
  bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
  bgfx::init(bgfxInit);

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
  bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);

  std::vector<Point> Vertices;
  std::vector<uint16_t> TriList;
  HexagonGrid tmp(0.3f, Point(-2.0f, -2.0f, 0.0f, color(255, 255, 255)),
    Vector(1, 0, 0), Vector(0, 0, 1), 10, 10);
  tmp.set_random_colors();
  tmp.print_in_vertices_and_triList(Vertices, TriList);

  bgfx::VertexLayout pcvDecl;
  pcvDecl.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
  bgfx::VertexBufferHandle vbh = 
    bgfx::createVertexBuffer(
      bgfx::makeRef(Vertices.data(),
        Vertices.size() * sizeof(Point)),
      pcvDecl);
  bgfx::IndexBufferHandle ibh = 
    bgfx::createIndexBuffer(
      bgfx::makeRef(TriList.data(),
        TriList.size() * sizeof(uint16_t)));

  bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
  bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

  unsigned int counter = 0;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);

    //Вращение TODO разобраться как сделать на заданный угол
    /*float mtx[16];
    bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
    bgfx::setTransform(mtx);*/

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);

    bgfx::submit(0, program);
    bgfx::frame();
    counter++;
  }

  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}