#pragma once

class EGLService {
public:
	void Initialise(ANativeWindow *window);
	void Shutdown();
	void SwapBuffers();

	EGLDisplay display = EGL_NO_DISPLAY;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;
	int32_t width = 0, height = 0;
};
