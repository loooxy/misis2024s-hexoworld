#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#define Debug(x) std::cout << #x << " = " << x << std::endl;
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "GLFW/glfw3.h"
#include <imgui.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "GLFW/glfw3native.h"

#include<hexoworld/hexoworld.hpp>
#include <hexoworld/texture_grid.hpp>
#include <hexoworld/hexagon_grid.hpp>

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

bgfx::ShaderHandle loadShader(std::string filename)
{
  std::string path = "../shaders/";

  switch (bgfx::getRendererType()) {
  case bgfx::RendererType::Noop:
  case bgfx::RendererType::Direct3D11:
  case bgfx::RendererType::Direct3D12: path += "dx11/";  break;
  case bgfx::RendererType::Gnm:        path += "pssl/";  break;
  case bgfx::RendererType::Metal:      path += "metal/"; break;
  case bgfx::RendererType::OpenGL:     path += "glsl/";  break;
  case bgfx::RendererType::OpenGLES:   path += "essl/";  break;
  case bgfx::RendererType::Vulkan:     path += "spirv/"; break;
  }

  path += filename;

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
Eigen::Vector3d at = { -11.0f, 4.0f,  14.0f };
Eigen::Vector3d eye = { -11.0f, 4.0f, 15.0f };
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

Hexoworld generateField() {
  Hexoworld tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(0, 0, -1), Eigen::Vector3d(-1, 0, 0), 6, 6);
  Eigen::Vector4i grass(53, 200, 45, 255);
  Eigen::Vector4i sand(252, 221, 50, 255);
  Eigen::Vector4i sea(0, 100, 255, 255);
  Eigen::Vector4i snow(255, 255, 255, 255);

  Eigen::Matrix<int, 6, 6> colors = (Eigen::MatrixXi(6, 6) <<
    1, 2, 1, 2, 2, 2,
    0, 2, 1, 1, 2, 1,
    0, 1, 1, 1, 3, 0,
    1, 0, 0, 0, 0, 0,
    1, 3, 3, 0, 1, 3,
    2, 3, 3, 3, 0, 2).finished();

  Eigen::Matrix<int, 6, 6> heights = (Eigen::MatrixXi(6, 6) <<
    1, 0, 1, 0, 0, 0,
    2, 0, 1, 1, 0, 1,
    2, 1, 1, 1, 3, 2,
    1, 2, 2, 2, 2, 2,
    1, 3, 3, 2, 1, 3,
    0, 3, 3, 3, 2, 0).finished();

  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j)
    {
      switch (colors(i, j))
      {
      case 0: tmp.set_hex_color(i, j, grass); break;
      case 1: tmp.set_hex_color(i, j, sand); break;
      case 2: tmp.set_hex_color(i, j, sea); break;
      case 3: tmp.set_hex_color(i, j, snow); break;
      }

      tmp.set_hex_height(i, j, heights(i, j));
    }
  return tmp;
}

int main()
{
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "HexoWorld", NULL, NULL);
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



  Hexoworld tmp = generateField();
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