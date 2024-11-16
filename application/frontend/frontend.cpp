#include "frontend.hpp"
#include <cereal/archives/portable_binary.hpp>

// TODO: add vector<Camera> players;

void Frontend::SetDataFromReply(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) {
  data.set(Vertices, TriList);
}

void Frontend::GetDataToRequest(std::string& ev) {
  ev = saveEv(events.pop());
}

std::string saveEv(const std::shared_ptr<Event>& ev) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(ev);
  return oss.str();
}

std::vector<PrintingPoint> Frontend::GetVertices() {
  return Vertices;
}

std::vector<uint16_t> Frontend::GetTriList() {
  return TriList;
}

std::queue<std::pair<int, int>> Frontend::GetCommands() {
  return Commands;
}

void Frontend::SetIsClient(bool is_client) {
  Frontend::is_client = is_client;
}

Frontend::Frontend() 
{
  work_with_map = std::make_unique<WorkWithMap>();

  init_glfw();
  init_ImGui();
  init_Shaders_and_Buffers();
}

void Frontend::glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void Frontend::init_glfw()
{
  // glfw initialize and configure
      // -----------------------------
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw std::runtime_error("glfw not init");
  }
  /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   */

  glfwWindowHint(GLFW_SAMPLES, 4); // Anti-Aliasing MSAA

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
  glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif


  // glfw window creation
  // --------------------
  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

void Frontend::init_ImGui()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //ImGui::GetIO().ConfigViewportsNoAutoMerge = true;
  //ImGui::GetIO().ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 0.3f; // imgui transparency
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void Frontend::init_Shaders_and_Buffers()
{
  //glad: load all OpenGl functions pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  // build and compile our shader program
  // ------------------------------------
  //Bilikto's common shaders
  filledShader = std::make_unique<Shader>("../shaders/1_shader.vs", "../shaders/1_shader.fs");

  //Edited with geometry shaders
  meshShader = std::make_unique<Shader>("../shaders/3.3.shader.vs", "../shaders/3.3.shader.fs", "../shaders/3.3.shader.geom");

  // set up vertex data (and (buffers(s)) and configure vertex attributes
  // --------------------------------------------------------------------

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s)
  glBindVertexArray(VAO);

  data.get(Vertices, TriList);

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
}

Frontend::~Frontend()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
}

void Frontend::prepare_ImGui()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Create the docking environment
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Menu")) {
      if (ImGui::MenuItem("Exit")) {
        glfwSetWindowShouldClose(window, true);
      }
      if (ImGui::MenuItem("New")) {}
      if (ImGui::MenuItem("Open", "Ctrl+O")) {}
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  ImGui::PopStyleVar(3);

  ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

  ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();

  ImGui::ShowMetricsWindow();
  char buffer[50];
  ImGui::Begin("debug");
  HelpMarker("Right Click to open hex settings");
  for (int col = 0; col < n_cols; col++) {
    for (int row = 0; row < n_rows; row++) {
      if (row > 0)
        ImGui::SameLine();

      ImGui::PushID(row * (n_cols + 1) + col);

      sprintf(buffer, "Hex %d %d", row, col);

      ImGui::Button(buffer);
      if (ImGui::BeginPopupContextItem()) {
        ImGui::Text(buffer);

        //height

        int start_height = work_with_map->heights(row, col);
        int cur_height = start_height;
        ImGui::SliderInt("Height", &cur_height, -3, 4);
        if (start_height != cur_height) {
          events.push(std::make_shared<ChangeHeight>(row, col, cur_height));
        }


        //color
        {
          int cell_color = work_with_map->colors(row, col);
          int n_colors = work_with_map->Colors_COUNT;

          const char* cell_color_name;
          if (cell_color >= 0 &&
            cell_color < n_colors)
            cell_color_name = work_with_map->elems_names[cell_color];
          else
            cell_color_name = "Unknown";

          int cur_color = cell_color;
          ImGui::SliderInt("Biom", &cur_color, 0, n_colors - 1, cell_color_name);
          if (cell_color != cur_color) {
            events.push(std::make_shared<ChangeColor>(row, col, cur_color));
          }
        }

        //road
        {
          bool road_state = work_with_map->roads(row, col);
          bool cur_road_state = road_state;
          ImGui::Checkbox("Road", &cur_road_state);
          if (road_state != cur_road_state) {
            events.push(std::make_shared<ChangeRoadState>(row, col, cur_road_state));
          }
        }

        ImGui::SameLine();

        //farm
        {
          bool farm_state = work_with_map->farms(row, col);
          bool cur_farm_state = farm_state;
          ImGui::Checkbox("Farm", &cur_farm_state);
          if (farm_state != cur_farm_state) {
            events.push(std::make_shared<ChangeFarmState>(row, col, cur_farm_state));
          }
        }

        //flood
        {
          bool flood_state = work_with_map->flood(row, col);
          bool cur_flood_state = flood_state;
          ImGui::Checkbox("flood(in process)", &cur_flood_state);
          if (flood_state != cur_flood_state) {
            events.push(std::make_shared<ChangeFloodState>(row, col, cur_flood_state));
          }
        }

        if (ImGui::Button("Close"))
          ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
      }
      ImGui::PopID();
    }
  }
  ImGui::Checkbox("Mesh Render", &is_changed_shader);
  ImGui::End();
}

void Frontend::prepare_window()
{
  // per-frame time logic
    // --------------------
  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  // input
  // -----
  if (!is_client) {
    processInput(window);
  }
  else {
    processInputInQueue(window);
  }

  // render
  // -----
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // shader handling
  // -----
  if (is_changed_shader) {
    meshShader->use();
  }
  else {
    filledShader->use();
  }

  // pass projection shader to the shader (in that case it should change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 800.0f);
  filledShader->setMat4("projection", projection);
  meshShader->setMat4("projection", projection);

  // camera/view transformation
  glm::mat4 view = camera.GetViewMatrix();
  filledShader->setMat4("view", view);
  meshShader->setMat4("view", view);
}

void Frontend::render_window()
{
  if (data.check())
  {
    data.get(Vertices, TriList);
    // map updating
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TriList[0]) * TriList.size(), TriList.data(), GL_STATIC_DRAW);
  }

  // draw map
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, TriList.size(), GL_UNSIGNED_SHORT, 0);
}

void Frontend::render_ImGui()
{
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void Frontend::work()
{
  while (!glfwWindowShouldClose(window)) {
    prepare_ImGui();
    prepare_window();

    render_window();
    render_ImGui();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  events.push(std::make_shared<Close>());
}

// process all input : query GLFW whether relevant keys are pressed / released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Frontend::processInput(GLFWwindow * window) {
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
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, deltaTime);
}

// for adding input commands in queue
void Frontend::processInputInQueue(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = static_cast<float>(2.5 * deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    Commands.push({ FORWARD, deltaTime });
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    Commands.push({ BACKWARD, deltaTime });
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    Commands.push({ LEFT, deltaTime });
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    Commands.push({ RIGHT, deltaTime });
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    Commands.push({ DOWN, deltaTime });
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    Commands.push({ UP, deltaTime });
}

// glfw: whenver the window size changed this callback function executes
// ---------------------------------------------------------------------
void Frontend::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Frontend::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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
  else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Frontend::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
