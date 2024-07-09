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
Eigen::Vector3d at = { 0.0f, 0.0f,  4.0f };
Eigen::Vector3d eye = { 0.0f, 0.0f, 5.0f };
Eigen::Vector3d rotating_angles = { 0, 0, 0 };
void rotate(Eigen::Vector3d normal, float degree) {
  Eigen::Vector3d a = (at - eye).normalized();
  Eigen::Vector3d b = a.cross(normal).normalized();
  at += a * cos(degree) + b * sin(degree);
}
void move(Eigen::Vector3d direction, float step)
{
  direction.normalize();
  at += direction * step;
  eye += direction * step;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  if (true)
  {
    Eigen::Vector3d view = at - eye;
    Eigen::Vector3d normal(0, 1, 0);
    switch (key)
    {
    case GLFW_KEY_K:
      move(view, -1);
      break;
    case GLFW_KEY_I:
      move(view, 1);
      break;
    case GLFW_KEY_J:
      move(view.cross(normal), 1);
      break;
    case GLFW_KEY_L:
      move(view.cross(normal), -1);
      break;
    case GLFW_KEY_U:
      move(normal, 1);
      break;
    case GLFW_KEY_O:
      move(normal, -1);
      break;
    case GLFW_KEY_A:
      rotating_angles += Eigen::Vector3d(0, 1, 0);
      rotate(normal, 1);
      break;
    case GLFW_KEY_D:
      rotating_angles += Eigen::Vector3d(0, 1, 0);
      rotate(normal, -1);
      break;
    case GLFW_KEY_W:
      if (rotating_angles.z() < 5)
      {
        rotating_angles += Eigen::Vector3d(0, 0, 0.1);
        rotate(view.cross(normal).normalized(), 0.1);
      }
      break;
    case GLFW_KEY_S:
      if (rotating_angles.z() > -5)
      {
        rotating_angles += Eigen::Vector3d(0, 0, -0.1);
        rotate(view.cross(normal).normalized(), -0.1);
      }
      break;
    }
  }
}
bx::Vec3 conwert_vector(Eigen::Vector3d v) {
  return bx::Vec3(v.x(), v.y(), v.z());
}

HexagonGrid generateField() {
  HexagonGrid tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(1, 0, 0), Eigen::Vector3d(0, 0, -1), 9, 9);

  tmp.generate_random_field();
  for (int i = 0; i < 9; ++i)
  {
    tmp.set_height(i, 0, 0);
    tmp.set_height(0, i, 0);
    tmp.set_height(i, 8, 0);
    tmp.set_height(8, i, 0);

    tmp.set_height(i, 1, 0);
    tmp.set_height(1, i, 0);
    tmp.set_height(i, 7, 0);
    tmp.set_height(7, i, 0);
  }
  return tmp;
}


int main()
{
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "Hello, bgfx!", NULL, NULL);
  glfwSetKeyCallback(window, key_callback);

  bgfx::PlatformData pd;
  pd.nwh = glfwGetWin32Window(window);
  bgfx::setPlatformData(pd);

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



  HexagonGrid tmp = generateField();
  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  tmp.print_in_vertices_and_triList(Vertices, TriList);

  bgfx::VertexLayout pcvDecl;
  pcvDecl.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
  bgfx::VertexBufferHandle vbh =
    bgfx::createVertexBuffer(
      bgfx::makeRef(Vertices.data(),
        Vertices.size() * sizeof(PrintingPoint)),
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

    float view[16];
    bx::mtxLookAt(view, conwert_vector(eye), conwert_vector(at));

    float proj[16];
    bx::mtxProj(proj, 60.0f,
      float(WNDW_WIDTH) / float(WNDW_HEIGHT),
      0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);


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