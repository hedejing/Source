#pragma once
#include "InteractionControler.h"
#include "app_edushi.h"
#include "SSAOPipeline.h"
class edushi_key {
public:
	App_edushi* app;
	GLFWwindow* window;
	edushi_key() {
		window = InteractionControler::window;
	}

	void SwitchLogCam(int key,  int action) {
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			app->IsLogCamera = !app->IsLogCamera;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			auto p =static_cast<EdushiPipeline*>(app->pipeline);
			p->ssao.radius *= 0.9;
			cout << "ao r:"<<p->ssao.radius << endl;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			auto p = static_cast<EdushiPipeline*>(app->pipeline);
			p->ssao.radius *= 1.1;
			cout << "ao r:" << p->ssao.radius << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			auto p = static_cast<EdushiPipeline*>(app->pipeline);
			p->tonemap.exposure *= 0.9;
			cout << "tonemap.exposure:" << p->tonemap.exposure << endl;
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			auto p = static_cast<EdushiPipeline*>(app->pipeline);
			p->tonemap.exposure *= 1.1;
			cout << "tonemap.exposure:" << p->tonemap.exposure << endl;
		}
	}
};
