#include "AndroidFramework.h"
#include "App_PBR.h"

using namespace std;


AndroidFramework::AndroidFramework(android_app *state) : app(state) {

	egl_service = new EGLService;
	app->onAppCmd = CommandHandler;
	app->onInputEvent = InputHandler;
	state->userData = this;
	glGetError();
	Renderer = new App_PBR;
	
}
void AndroidFramework::Init() {
	print_log(ANDROID_LOG_DEBUG, "Startup");
	egl_service->Initialise(app->window);
	Renderer->w = egl_service->width;
	Renderer->h = egl_service->height;
	Renderer->Init();
}
AndroidFramework::~AndroidFramework() {

	egl_service->Shutdown();
	delete egl_service;

}

void AndroidFramework::Run() {

	while (app->destroyRequested == 0) {
	
		android_poll_source *source;
		while (ALooper_pollAll(0, nullptr, nullptr, reinterpret_cast<void **>(&source)) >= 0) {
			if (source != nullptr)
				source->process(app, source);
		}
		if (!state_ok) continue;

		Renderer->Render();
		egl_service->SwapBuffers();

	}

	Shutdown();

	print_log(ANDROID_LOG_DEBUG, "Shutdown");
}



void AndroidFramework::Shutdown() {
	egl_service->Shutdown();
}




void AndroidFramework::CommandHandler(struct android_app *state, int32_t cmd) {
	auto app = static_cast<AndroidFramework *>(state->userData);
	switch (cmd) {
	case APP_CMD_INIT_WINDOW:
		print_log(ANDROID_LOG_DEBUG, "Renderer resumed");
		app->Init();
		app->state_ok = true;
		break;

	case APP_CMD_TERM_WINDOW:
		print_log(ANDROID_LOG_DEBUG, "Renderer suspended");

		break;
	}
}

int32_t AndroidFramework::InputHandler(struct android_app *state, AInputEvent *event) {
	return 0;
}