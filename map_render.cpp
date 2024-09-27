#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_bgfx.cpp>
#include <imgui_impl_glfw.cpp>

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
Camera camera(glm::vec3(0.0f, 20.0f, 0.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;


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

int main() {
  // glfw initialize and configure
  // -----------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scroll_callback);

  //glad: load all OpenGl functions pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // build and compile our shader program
  // ------------------------------------

  //Bilikto's common shaders
  Shader commonShader("../shaders/1_shader.vs", "../shaders/1_shader.fs");

  //Edited with geometry shaders
  Shader ourShader("../shaders/3.3.shader.vs", "../shaders/3.3.shader.fs", "../shaders/3.3.shader.geom");

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 4 * sizeof(byte), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 3 * sizeof(float) + 4 * sizeof(byte), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // render loop
 // -----------
  while (!glfwWindowShouldClose(window)) {
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

    // activate shader
    commonShader.use();
    ourShader.use();

    // pass projection shader to the shader (in that case it should change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    commonShader.setMat4("projection", projection);
    ourShader.setMat4("projection", projection);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();
    commonShader.setMat4("view", view);
    ourShader.setMat4("view", view);

    // draw map
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, TriList.size(), GL_UNSIGNED_SHORT, 0);

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
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
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

