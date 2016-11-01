#include "InteractionControler.h"


Camera* InteractionControler::camera;

GLFWwindow* InteractionControler::window;

CameraRotate InteractionControler::cameraRotate;

CameraMove InteractionControler::cameraMove;

vector<function<void(int, int)>> InteractionControler::KeyCallBacks;

GUI* InteractionControler::gui;

