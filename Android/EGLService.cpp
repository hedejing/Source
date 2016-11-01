#pragma once

#include "EGLService.h"

void EGLService::Initialise(ANativeWindow *window) {

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		print_log(ANDROID_LOG_ERROR, "Renderer::EGLService::Initialise -> cannot open connection to local windowing system");
		return;
	}

	if (eglInitialize(display, nullptr, nullptr) == EGL_FALSE) {
		print_log(ANDROID_LOG_ERROR, "Renderer::EGLService::Initialise -> cannot initialize EGL");
		return;
	}

	const EGLint config_attribs[] = {
		EGL_RENDERABLE_TYPE, 0x0040,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE
	};
	EGLint num_config;
	EGLConfig config;
	if (eglChooseConfig(display, config_attribs, &config, 1, &num_config) == EGL_FALSE) {
		print_log(ANDROID_LOG_ERROR, "Renderer::EGLService::Initialise -> cannot find configuration");
		return;
	}

	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, window, nullptr);
	if (surface == EGL_NO_SURFACE) {
		print_log(ANDROID_LOG_ERROR, "Renderer::EGLService::Initialise -> cannot create window surface");
		return;
	}

	const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
	if (context == EGL_NO_CONTEXT) {
		print_log(ANDROID_LOG_ERROR, "Renderer::EGLService::Initialise -> cannot create context");
		return;
	}

	eglMakeCurrent(display, surface, surface, context);

	eglQuerySurface(display, surface, EGL_WIDTH, &width);
	eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void EGLService::Shutdown() {
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}

	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;
}

void EGLService::SwapBuffers() {
	eglSwapBuffers(display, surface);
}
