#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include "GUIWrapper.h"
using namespace HW;
using namespace std;

class CameraSpeed {
public:
	float speed_base = 0.0005;
	int level = 5;
	int MinLevel = 1;
	int MaxLevel = 10;
	float speed=1.0;

	CameraSpeed() {
		CalcSpeed();
	}

	void Dec(int d) {
		level -= d;
		if (level <= MinLevel)
			level = MinLevel;
		CalcSpeed();
	}
	void Inc(int d) {
		level += d;
		if (level >=MaxLevel)
			level = MaxLevel;
		CalcSpeed();
	}
	void CalcSpeed() {
		speed=speed_base*level*level*level;
	}
};

class CameraRotate {
public:
	//for camera rotate
	 bool EnterRotateMode=false;
	 bool firstMove=true;
	 float lastx;
	 float lasty;
	 int w, h;
	 float RotateSpeed=100;

	 void process_mouse_button(int button, int  action) {
		 if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			 EnterRotateMode = true;
			 firstMove = true;
		 }
		 if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			 EnterRotateMode = false;
		 }
	 }
	 void process_mouse_move(double x,double y,Camera* camera) {
		 if (!EnterRotateMode) return;
		 if (firstMove) {
			 lastx = x;
			 lasty = y;
			 firstMove = false;
			 return;
		 }
		 float dx = (x - lastx) / w*RotateSpeed;
		 float dy = (y - lasty) / h*RotateSpeed;
		 lastx = x;
		 lasty = y;
		 camera->RotatePitchYaw(-dy, dx);
		 camera->setUp(Vector3(0, 1, 0));
	 }
};

class CameraMove {
public:
	CameraSpeed MoveSpeed;
	double LastTime;
	void move_func(GLFWwindow* window,Camera* camera) {
		double CurrentTime = glfwGetTime();
		auto DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;
		float d = MoveSpeed.speed;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera->MoveUp(d);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera->MoveDown(d);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->MoveForward(d);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->MoveBack(d);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->MoveLeft(d);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->MoveRight(d);
		}
	}

	void move_speed_change(int key ,int action) {
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
			MoveSpeed.Inc(1);
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
			MoveSpeed.Dec(1);
	}
};

class InteractionControler
{
public:
	static Camera* camera;
	static GLFWwindow* window;	
	static CameraRotate cameraRotate;
	static CameraMove cameraMove;
	static vector<function<void(int, int)>> KeyCallBacks;
	static GUI* gui;
	static void SetCamera(Camera* cam) {
		camera = cam;
	}

	static void SetWindow(GLFWwindow* win) {
		window = win;
		glfwGetWindowSize(win, &cameraRotate.w, &cameraRotate.h);
	}
	static void key_func() {
		cameraMove.move_func(window, camera);
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		cameraMove.move_speed_change(key, action);
		gui->MouseKeyCallBack(window, key, scancode, action, mode);
		for (auto& f : KeyCallBacks) {
			f(key, action);
		}
	}
	static void mouse_button_callback(GLFWwindow* window, int button, int  action,int mod) {
		cameraRotate.process_mouse_button(button, action);
		gui->MouseButtonCallBack(window, button, action, mod);
	}

	static void mouse_move_callback(GLFWwindow* window, double x, double y) {
		cameraRotate.process_mouse_move(x, y,camera);
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		gui->MouseScrollCallBack(window, xoffset, yoffset);
		cout << "scroll" << xoffset << " "<<yoffset << endl;
	}

	static void char_callback(GLFWwindow* window, unsigned ch) {
		gui->MouseCharCallBack(window, ch);
	}

	static void iconify_callback(GLFWwindow* window, int f) {
		cout << "iconify "<<f<<endl;
	}
	static void focus_callback(GLFWwindow* window, int f) {
		cout <<"focus "<< f << endl;
		//if(f==1)
	}
};
