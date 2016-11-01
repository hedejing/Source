#pragma once
#include "EGLService.h"
#include "AppFrameWork2.h"

class AndroidFramework {

public:
	AndroidFramework(android_app *state);
	~AndroidFramework();

	void Run();
	EGLService *egl_service = nullptr;

	void Init();
private:
	void Shutdown();
	static void CommandHandler(struct android_app *state, int32_t cmd);
	static int32_t InputHandler(struct android_app *state, AInputEvent *event);

	bool state_ok = false;
	android_app *app = nullptr;
	App* Renderer;
	int w, h;

};