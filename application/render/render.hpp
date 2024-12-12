#pragma once
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include<hexoworld/includes.hpp>
#include <events/events.hpp>
#include <events_queue/events_queue.hpp>
#include <data_pool/data_pool.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opengl/camera/camera.hpp>
#include <opengl/shader_s/shader_s.hpp>

#include <queue>

class Render {
public:
	explicit Render();
	~Render();
	void work();

	std::shared_ptr<Event> GetEvent();
	void UpdateMap(const std::shared_ptr<Event>& event);
	void UpdateData();

	void InitMap(std::string& map);
	void InitMapBasis(std::string& map_basis);

private:
	void init_glfw();
	void init_ImGui();
	void init_Shaders_and_Buffers();

	void prepare_ImGui();
	void prepare_window();
	void render_ImGui();
	void render_window();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput(GLFWwindow* window);
	static void processInputInQueue(GLFWwindow* window);

	// imgui helpmarker
	void HelpMarker(const char* desc)
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
	static void glfw_error_callback(int error, const char* description);

	static const unsigned int SCR_WIDTH = 1920;
	static const unsigned int SCR_HEIGHT = 1080;

	// camera
	static Camera camera;
	static bool firstMouse;
	static float lastX;
	static float lastY;

	// timing
	static float deltaTime; // time between current frame and last frame
	static float lastFrame;

	GLFWwindow* window;
	std::unique_ptr<Shader> filledShader;
	std::unique_ptr<Shader> meshShader;
	unsigned int VBO, VAO, EBO;
	const char* glsl_version;
	bool is_changed_shader = false;

	std::vector<PrintingPoint> Vertices;
	std::vector<uint16_t> TriList;
	data_pool data;

	// queue commands
	static std::queue < std::pair<int, int> > Commands;
	events_queue<Event> events;

	std::shared_ptr<WorkWithMap> work_with_map;
};