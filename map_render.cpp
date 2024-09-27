#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define Debug(x) std::cout << #x << " = " << x << std::endl;

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include<hexoworld/includes.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opengl/camera.h>
#include <opengl/shader_s.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(-60.0f, 15.0f, 50.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// imgui helpmarker
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
                Eigen::Vector3d(0, 0, -2), Eigen::Vector3d(-1, 0, 0), 1, 2, 10, 10);

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

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
  // glfw initialize and configure
  // -----------------------------
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()){
    return 1;
  }
  /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   */


  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif


  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);

  //glad: load all OpenGl functions pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader program
  // ------------------------------------
  //Bilikto's common shaders
  Shader filledShader("../shaders/1_shader.vs", "../shaders/1_shader.fs");

  //Edited with geometry shaders
  Shader meshShader("../shaders/3.3.shader.vs", "../shaders/3.3.shader.fs", "../shaders/3.3.shader.geom");

  // set up vertex data (and (buffers(s)) and configure vertex attributes
  // --------------------------------------------------------------------
  Hexoworld tmp = generateField();
  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  tmp.print_in_vertices_and_triList(Vertices, TriList);

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s)
  glBindVertexArray(VAO);

  // bind VBO for Vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

  // bind EBO for indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TriList[0]) * TriList.size(), TriList.data(), GL_STATIC_DRAW);

  // positon attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 4 * sizeof(std::byte), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 3 * sizeof(float) + 4 * sizeof(std::byte), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowMetricsWindow();
    char buffer[50];
    ImGui::Begin("debug");
    bool is_changed_height = false;
    bool is_changed_biom = false;
    bool is_changed_road = false;
    bool is_changed_farm = false;
    HelpMarker("Right Click to open hex settings");
    for(int col = 0; col < tmp.get_n_cols(); col++) {
      for(int row = 0; row < tmp.get_n_rows(); row++) {

        if (row > 0)
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
            if (roads(row, col) == true) {
              tmp.add_road_in_hex(row, col);
            }
            else {
              tmp.del_road_in_hex(row, col);
            }
          }
          ImGui::SameLine();
          bool farm_state = farms(row,col);
          ImGui::Checkbox("Farm", &farms(row,col));
          if(farm_state != farms(row, col)) {
            is_changed_farm = true;
            if (farms(row, col) == true) {
              tmp.add_farm_in_hex(row, col);
            }
            else {
              tmp.del_farm_in_hex(row, col);
            }
          }

          if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
          ImGui::EndPopup();
        }
        ImGui::PopID();
      }
    }
    static bool is_changed_shader = false;
    ImGui::Checkbox("Mesh Render", &is_changed_shader);
    ImGui::End();

    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // -----
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // shader handling
    // -----
    if(is_changed_shader) {
      meshShader.use();
    }else {
      filledShader.use();
    }

    // pass projection shader to the shader (in that case it should change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    filledShader.setMat4("projection", projection);
    meshShader.setMat4("projection", projection);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();
    filledShader.setMat4("view", view);
    meshShader.setMat4("view", view);

    // map updating
    if(is_changed_height || is_changed_biom || is_changed_road || is_changed_farm) {
      tmp.print_in_vertices_and_triList(Vertices, TriList);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TriList[0]) * TriList.size(), TriList.data(), GL_STATIC_DRAW);
    }

    // draw map
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, TriList.size(), GL_UNSIGNED_SHORT, 0);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    // glfw: check and call events and swap buffers
    // --------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = static_cast<float>(2.5 * deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenver the window size changed this callback function executes
// ---------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  if (state == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    camera.ProcessMouseMovement(xoffset, yoffset);
  }
  else{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
