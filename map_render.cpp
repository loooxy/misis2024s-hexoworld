#include <stdio.h>
#include <string>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_bgfx.cpp>
#include <imgui_impl_glfw.cpp>
#include <thread>
#include <vector>
#include <chrono>
#define Debug(x) std::cout << #x << " = " << x << std::endl;
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "GLFW/glfw3.h"
#include <vector>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "GLFW/glfw3native.h"


#include<hexoworld/includes.hpp>

#define WNDW_WIDTH 2880
#define WNDW_HEIGHT 1880

std::vector<std::vector<int>> check(4, std::vector<int>(4));

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
/*Eigen::Vector3d at = { -25.8, 0,  -21};
Eigen::Vector3d eye = { -25.9, 50, -21};*/
Eigen::Vector3d at = { -78, 14, -22 };
Eigen::Vector3d eye = { -85.5, 14, -22.5 };
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

Eigen::Vector4i grass(53, 200, 45, 255);
Eigen::Vector4i sand(252, 221, 50, 255);
Eigen::Vector4i sea(0, 100, 255, 255);
Eigen::Vector4i snow(255, 255, 255, 255);
Eigen::Vector4i mount(181, 184, 177, 255);
Eigen::Vector4i test(219, 255, 255, 255);

enum ColorsName
{
  se, sa, gr, mo, sn, te, Colors_COUNT
};

Eigen::Matrix<int, 10, 10> colors = (Eigen::MatrixXi(10, 10) <<
    mo, mo, sn, sn, sn, mo, mo, se, se, se,
    gr, gr, mo, mo, mo, sa, mo, se, se, se,
    sa, sa, gr, gr, gr, sa, sa, gr, se, se,
    mo, gr, sa, sa, sa, gr, sa, gr, gr, gr,
    mo, gr, gr, sa, sa, sa, gr, sa, sa, gr,
    mo, mo, gr, se, se, se, gr, sa, gr, gr,
    mo, mo, se, se, se, sa, gr, gr, sa, gr,
    mo, mo, se, se, se, sa, gr, gr, se, se,
    mo, mo, gr, gr, se, sa, gr, gr, se, sa,
    sn, mo, mo, mo, gr, gr, gr, gr, sa, sa).finished();

Eigen::Matrix<int, 10, 10> heights = (Eigen::MatrixXi(10, 10) <<
   3,  3,  4,  4,  4,  3,  2, -1, -2, -3,
   1,  1,  2,  2,  3,  1,  2, -1, -2, -2,
   0,  0,  1,  1,  1,  0,  0,  1, -1, -1,
   2,  1,  0,  0,  0,  1,  0,  1,  1,  0,
   3,  1,  1,  1,  0,  0,  1,  0,  0,  1,
   2,  3,  0, -1, -1, -1,  1,  0,  1,  1,
   3,  2, -1, -2, -2,  0,  0,  1,  0,  1,
   3,  2, -1, -1, -1,  0,  1,  0, -1, -2,
   2,  3,  1,  0, -1,  0,  1,  0, -1,  1,
   4,  3,  3,  2,  0,  1,  1,  0,  1,  1).finished();



Eigen::Matrix<bool, 10, 10> roads = (Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>(10, 10) <<
   false,  false,  false,  false,  false,  false,  false, false, false, false,
   false,  false,  false,  false,  false,  false,  false, false, false, false,
   false,  false,  false,  false,  false,  false,  false,  false, false, false,
   false,  false,  false,  false,  false,  true,  true,  false,  false,  false,
   false,  false,  false,  false,  false,  false,  true,  false,  false,  false,
   false,  false,  false, false, false, false,  false,  false,  false,  false,
   false,  false, false, false, false,  false,  false,  false,  false,  false,
   false,  false, false, false, false,  false,  false,  false, false, false,
   false,  false,  false,  false, false,  false,  false,  false, false, false,
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false).finished();


Eigen::Matrix<bool, 10, 10> farms = (Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>(10, 10) <<
   false,  false,  false,  false,  false,  false,  false, false, false, false,
   false,  false,  false,  false,  false,  false,  false, false, false, false,
   false,  false,  false,  false,  false,  false,  false,  false, false, false,
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
   false,  false,  false,  false,  false,  false,  false,  false,  false,  true,
   false,  false,  false, false, false, false,  false,  false,  false,  false,
   false,  false, false, false, false,  false,  false,  false,  false,  false,
   false,  false, false, false, false,  false,  false,  false, false, false,
   false,  false,  false,  false, false,  false,  false,  false, false, false,
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false).finished();



Hexoworld generateField() {
  Hexoworld tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(0, 0, -1), Eigen::Vector3d(-1, 0, 0), 1, 2, 10, 10);

  tmp.add_river({
    {0, 3}, {1, 2}, {2, 3}, {3, 2}, {3, 3}, {4, 4}, {5, 4}
    });
  tmp.add_road_in_hex(4, 6);
  tmp.add_road_in_hex(3, 5);
  tmp.add_road_in_hex(3, 6);

  tmp.add_farm_in_hex(4, 9);

  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
    {
      switch (colors(i, j))
      {
      case gr: tmp.set_hex_color(i, j, grass); break;
      case sa: tmp.set_hex_color(i, j,  sand); break;
      case se: tmp.set_hex_color(i, j,   sea); break;
      case sn: tmp.set_hex_color(i, j,  snow); break;
      case mo: tmp.set_hex_color(i, j, mount); break;
      case te: tmp.set_hex_color(i, j,  test); break;
      }

      tmp.set_hex_height(i, j, heights(i, j));
    }




  return tmp;
}

static void HelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip())
  {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

int main() {
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

  bgfx::VertexLayout pcvDecl;
  pcvDecl.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();

  bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
  bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

  Hexoworld tmp = generateField();
  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  tmp.print_in_vertices_and_triList(Vertices, TriList);

  bgfx::IndexBufferHandle ibh =
    bgfx::createIndexBuffer(
      bgfx::makeRef(TriList.data(),
        TriList.size() * sizeof(uint16_t)));

  bgfx::VertexBufferHandle vbh =
    bgfx::createVertexBuffer(
      bgfx::makeRef(Vertices.data(),
        Vertices.size() * sizeof(PrintingPoint)),
      pcvDecl);


  bgfx::setVertexBuffer(0, vbh);
  bgfx::setIndexBuffer(ibh);

  unsigned int counter = 0;
  std::shared_ptr<std::thread> print_thread = nullptr;

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui_Implbgfx_Init(255);
  ImGui_ImplGlfw_InitForOther(window, true);

  while (!glfwWindowShouldClose(window)) {
    counter++;
    auto begin = std::chrono::steady_clock::now();

    ImGui_Implbgfx_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
    /*
    ImGuiTableFlags flags2 = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("table_context_menu_2", tmp.get_cols(), flags2)) {
      // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
      for (int row = 0; row < tmp.get_rows(); row++)
      {
        ImGui::TableNextRow();
        for (int column = 0; column < tmp.get_cols(); column++)
        {
          // Submit dummy contents
          ImGui::TableSetColumnIndex(column);
          ImGui::Text("Cell %d,%d", column, row);
          ImGui::SameLine();

          // [2.2] Right-click on the ".." to open a custom popup
          ImGui::PushID(row * tmp.get_cols() + column);
          ImGui::SmallButton("..");
          if (ImGui::BeginPopupContextItem())
          {
            ImGui::Text("This is the popup for Button(\"..\") in Cell %d,%d", column, row);
            int iop = 0;
            ImGui::SliderInt("slider int", &iop, -3, 4);
            ImGui::SameLine();
            if (ImGui::Button("Close")) {
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
          }
          ImGui::PopID();
        }
      }


      ImGui::EndTable();
    }
    */
    /*std::vector<std::vector<bool>> selected(tmp.get_n_rows(), std::vector<bool>(tmp.get_n_cols(), 0));
    static char selected1[10][10] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    // Add in a bit of silly fun...
    const float time = (float)ImGui::GetTime();
    char buffer [50];
    for (int row = 0; row < tmp.get_n_rows(); row++)
      for (int col = 0; col < tmp.get_n_cols(); col++)
      {
        if (col > 0)
          ImGui::SameLine();
        ImGui::PushID(row * (tmp.get_n_cols() + 1) + col);
        sprintf (buffer, "ceil %d %d", row, col);
        if (ImGui::Selectable(buffer, selected[row][col] != 0, 0, ImVec2(70, 70)))
        {
          // Toggle clicked cell + toggle neighbors
          selected[row][col] = (selected[row][col] == 1 ? 0 : 1);
        }
        if(selected[row][col] == 1) {
          ImGui::Begin(buffer);

          ImGui::SliderInt("check", &check[row][col], 0, 2);
          ImGui::End();
          /*if (ImGui::BeginPopupContextItem())
          {
            ImGui::Text("This is the popup for Butt in Cell %d,%d", col, row);
            int iop = 0;
            ImGui::SliderInt("slider int", &iop, -3, 4);
            ImGui::SameLine();
            if (ImGui::Button("Close")) {
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
          }#1#
        }
        ImGui::PopID();
      }
      */
    ImGui::ShowMetricsWindow();
    char buffer[50];
    ImGui::Begin("debug");
    bool is_changed_height = false;
    bool is_changed_biom = false;
    bool is_changed_road = false;
    bool is_changed_farm = false;
    HelpMarker("Right Click to open hex settings");
    for(int row = 0; row < tmp.get_n_rows(); row++) {
      for(int col = 0; col < tmp.get_n_cols(); col++) {

        if (col > 0)
          ImGui::SameLine();
        ImGui::PushID(row * (tmp.get_n_cols() + 1) + col);
        sprintf (buffer, "Hex %d %d", row, col);
        ImGui::Button(buffer);
        if(ImGui::BeginPopupContextItem()) {
          ImGui::Text(buffer);
          int temp = heights(row,col);
          ImGui::SliderInt("Height", &heights(row,col), -3, 4);
          if(temp != heights(row, col)) {
            is_changed_height = true;
            tmp.set_hex_height(row,col,heights(row,col));
          }
          int elem = colors(row,col);  
          const char* elems_names[Colors_COUNT] =  {"Sea", "Sand", "Grass", "Mountain", "Snow", "Test"};
          const char* elem_name = (colors(row,col) >= 0 && colors(row,col) < Colors_COUNT) ? elems_names[colors(row,col)] : "Unknown";
          ImGui::SliderInt("Biom", &colors(row,col), 0, Colors_COUNT - 1, elem_name);
          if(elem != colors(row, col)) {
            is_changed_biom = true;
            switch (colors(row, col))
            {
              case gr: tmp.set_hex_color(row, col, grass); break;
              case sa: tmp.set_hex_color(row, col,  sand); break;
              case se: tmp.set_hex_color(row, col,   sea); break;
              case sn: tmp.set_hex_color(row, col,  snow); break;
              case mo: tmp.set_hex_color(row, col, mount); break;
              case te: tmp.set_hex_color(row, col,  test); break;
            }

          }

          bool road_state = roads(row,col);
          ImGui::Checkbox("Road", &roads(row,col));
          if(road_state != roads(row, col)) {
            is_changed_road = true;
            tmp.add_road_in_hex(row,col);
          }
          ImGui::SameLine();
          bool farm_state = farms(row,col);
          ImGui::Checkbox("Farm", &farms(row,col));
          if(farm_state != farms(row, col)) {
            is_changed_farm = true;
            tmp.add_farm_in_hex(row,col);
          }

          if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
          ImGui::EndPopup();
        }
        ImGui::PopID();
      }

    }
    ImGui::End();

    ImGui::Render();
    ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

    glfwPollEvents();



    float view[16];
    bx::mtxLookAt(view, conwert_vector(eye), conwert_vector(at));

    float proj[16];
    bx::mtxProj(proj, 60.0f,
      float(WNDW_WIDTH) / float(WNDW_HEIGHT),
      0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);


    static auto last_print = std::chrono::steady_clock::now();
    auto this_moment = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>
      (this_moment - last_print).count() > 100)
    {
      static bool was_init = false;
      if (was_init)
        print_thread->join();

      auto func = [&tmp, &Vertices, &TriList]()->void {
        tmp.update_river();
        tmp.print_in_vertices_and_triList(Vertices, TriList);
      };
      if(is_changed_height || is_changed_biom || is_changed_road || is_changed_farm) {
        tmp.print_in_vertices_and_triList(Vertices, TriList);
      }
      print_thread = std::make_shared<std::thread>(func);
      was_init = true;

      bgfx::destroy(vbh);
      bgfx::destroy(ibh);
      ibh =
        bgfx::createIndexBuffer(
          bgfx::makeRef(TriList.data(),
            TriList.size() * sizeof(uint16_t)));

      vbh =
        bgfx::createVertexBuffer(
          bgfx::makeRef(Vertices.data(),
            Vertices.size() * sizeof(PrintingPoint)),
          pcvDecl);


      last_print = this_moment;
    }


    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);
    bgfx::submit(0, program);
    bgfx::frame();

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::this_thread::sleep_for(std::chrono::milliseconds(std::max(0ll, 40 - elapsed_ms.count())));
  }
  print_thread->join();

  ImGui_Implbgfx_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  bgfx::destroy(vbh);
  bgfx::destroy(ibh);
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}


