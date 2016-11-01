#pragma once
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <functional>
#include "EngineUtil.h"
class GLFWwindow;
using namespace std;
class GUI {
public:
	std::function<void(GLFWwindow*,int,int,int)> MouseButtonCallBack;
	std::function<void(GLFWwindow*, double, double)> MouseScrollCallBack;
	std::function<void(GLFWwindow*, int,int, int, int)> MouseKeyCallBack;
	std::function<void(GLFWwindow*,unsigned int)> MouseCharCallBack;

	void Init(GLFWwindow* window) {
		ImGui_ImplGlfwGL3_Init(window, false);
		ImGuiIO& io = ImGui::GetIO();
		string fullpath = GetFullPath("Arial.ttf");
		io.Fonts->AddFontFromFileTTF(fullpath.c_str(), 15.0f);
		MouseButtonCallBack = ImGui_ImplGlfwGL3_MouseButtonCallback;
		MouseScrollCallBack = ImGui_ImplGlfwGL3_ScrollCallback;
		MouseKeyCallBack = ImGui_ImplGlfwGL3_KeyCallback;
		MouseCharCallBack = ImGui_ImplGlfwGL3_CharCallback;
	}

	static void NewFrame() {
		ImGui_ImplGlfwGL3_NewFrame();
	}

	void Render() {
		ImGui::Render();
	}

	void ShutDown() {
		ImGui_ImplGlfwGL3_Shutdown();
	}

};

/*
//example
ImGui_ImplGlfwGL3_NewFrame();

// 1. Show a simple window
// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
{
static float f = 0.0f;
ImGui::Begin("123");
ImGui::Text("Hello, world!");
ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

ImGui::ColorEdit3("clear color", (float*)&clear_color);
if (ImGui::Button("Test Window")) show_test_window ^= 1;
if (ImGui::Button("Another Window")) show_another_window ^= 1;
ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
ImGui::End();

}

// 2. Show another simple window, this time using an explicit Begin/End pair
if (show_another_window)
{
ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
ImGui::Begin("Another Window", &show_another_window);
ImGui::Text("Hello");
ImGui::End();
}

// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
if (show_test_window)
{
ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
ImGui::ShowTestWindow(&show_test_window);
}


*/