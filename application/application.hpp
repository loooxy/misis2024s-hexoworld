#pragma once

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
#include <queue>
#define Debug(x) std::cout << #x << " = " << x << std::endl;

class Application
{
public:
	Application();
	~Application();

	void work();
private:
	static class Frontend {
	public:
		Frontend(Application* app);
		~Frontend();
		static void work();
	private:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void processInput(GLFWwindow* window);

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
		static void glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "GLFW Error %d: %s\n", error, description);
		}

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

		static Application* app;
		static GLFWwindow* window;
		static std::unique_ptr<Shader> filledShader;
		static std::unique_ptr<Shader> meshShader;
		static unsigned int VBO, VAO, EBO;
	};

	class WorkWithMap {
	public:
		WorkWithMap(Application* app);
		~WorkWithMap();
		void work();

		int get_n_cols();
		int get_n_rows();
		Eigen::Vector4i get_color(int color_id);
		int heights(int row, int col);
		int colors(int row, int col);
		bool roads(int row, int col);
		bool farms(int row, int col);

		void set_hex_height(int row, int col, int new_height);
		void set_hex_color(int row, int col, int color_id);
		void set_road_state_in_hex(int row, int col, bool road_state);
		void set_farm_state_in_hex(int row, int col, bool farm_state);
		void update_river();

		enum ColorsName
		{
			se, sa, gr, mo, sn, te, Colors_COUNT
		};
		const char* elems_names[6];

	private:
		void generateField(std::shared_ptr<Hexoworld>& map);

		void regular_event_update_river();
		bool application_is_alive = true;

		Application* app;

		std::shared_ptr<Hexoworld> map;
		int n_rows, n_cols;

		Eigen::Vector4i grass; 
		Eigen::Vector4i sand;
		Eigen::Vector4i sea;
		Eigen::Vector4i snow;
		Eigen::Vector4i mount;
		Eigen::Vector4i test;

		Eigen::Matrix<int, 10, 10> colors_ = (Eigen::MatrixXi(10, 10) <<
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

		Eigen::Matrix<int, 10, 10> heights_ = (Eigen::MatrixXi(10, 10) <<
			3, 3, 4, 4, 4, 3, 2, -1, -2, -3,
			1, 1, 2, 2, 3, 1, 2, -1, -2, -2,
			0, 0, 1, 1, 1, 0, 0, 1, -1, -1,
			2, 1, 0, 0, 0, 1, 0, 1, 1, 0,
			3, 1, 1, 1, 0, 0, 1, 0, 0, 1,
			2, 3, 0, -1, -1, -1, 1, 0, 1, 1,
			3, 2, -1, -2, -2, 0, 0, 1, 0, 1,
			3, 2, -1, -1, -1, 0, 1, 0, -1, -2,
			2, 3, 1, 0, -1, 0, 1, 0, -1, 1,
			4, 3, 3, 2, 0, 1, 1, 0, 1, 1).finished();

		Eigen::Matrix<bool, 10, 10> roads_ = (Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>(10, 10) <<
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, true, true, false, false, false,
			false, false, false, false, false, false, true, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false).finished();


		Eigen::Matrix<bool, 10, 10> farms_ = (Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>(10, 10) <<
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, true,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false).finished();
	};

	enum TypeEvent {
		changeHeight, changeColor, changeRoadState, changeFarmState, updateRiver, close
	};
	class Event {
	public:
		Event(Application* app) : app(app) {}

		virtual TypeEvent type() = 0;
		virtual void execute(WorkWithMap* wwm) = 0;

		Application* app;
	};
	class ChangeHeight : public Event {
	public:
		ChangeHeight(Application* app, int row, int col, int new_height)
			: Event(app), row(row), col(col), new_height(new_height) {}
		TypeEvent type() { return changeHeight; }

		void execute(WorkWithMap* wwm) { wwm->set_hex_height(row, col, new_height); }

		int row, col, new_height;
	};
	class ChangeColor : public Event {
	public:
		ChangeColor(Application* app, int row, int col, int new_color)
			: Event(app), row(row), col(col), new_color(new_color) {}
		TypeEvent type() { return changeColor; }

		void execute(WorkWithMap* wwm) {
			wwm->set_hex_color(row, col, new_color);
		}

		int row, col, new_color;
	};
	class ChangeRoadState : public Event {
	public:
		ChangeRoadState(Application* app, int row, int col, bool road_state)
			: Event(app), row(row), col(col), road_state(road_state) {}

		TypeEvent type() { return changeRoadState; }

		void execute(WorkWithMap* wwm) {
				wwm->set_road_state_in_hex(row, col, road_state);
		}

		int row, col;
		bool road_state;
	};
	class ChangeFarmState : public Event {
	public:
		ChangeFarmState(Application* app, int row, int col, bool farm_state)
			: Event(app), row(row), col(col), farm_state(farm_state) {}

		TypeEvent type() { return changeFarmState; }

		void execute(WorkWithMap* wwm) {
			wwm->set_farm_state_in_hex(row, col, farm_state);
		}

		int row, col;
		bool farm_state;
	};
	class UpdateRiver : public Event {
	public:
		UpdateRiver(Application* app) : Event(app) {}

		TypeEvent type() { return updateRiver; }

		void execute(WorkWithMap* wwm) {
			wwm->update_river();
		}
	};
	class Close : public Event {
	public:
		Close(Application* app) : Event(app) {}

		TypeEvent type() { return close; }

		void execute(WorkWithMap* wwm) {}
	};


	std::shared_ptr<Frontend> frontend;
	std::shared_ptr<WorkWithMap> work_with_map;
	std::queue<std::shared_ptr<Event>> events;
	std::vector<PrintingPoint> Vertices;
	std::vector<uint16_t> TriList;
	bool need_update_buffers = true;
	std::mutex events_mtx, buffers_mtx;
};