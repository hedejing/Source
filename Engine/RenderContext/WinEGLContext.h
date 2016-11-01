#pragma once
#pragma comment(lib,"libEGL.lib")
#pragma comment(lib,"libGLESv2.lib")
#include <egl/egl.h>
#include <egl/eglext.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#include <windows.h>

class WinEGLContext {
public:
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
	int Init(HWND hwnd) {
		EGLint attribList[] =
		{
			EGL_RED_SIZE,       8,
			EGL_GREEN_SIZE,     8,
			EGL_BLUE_SIZE,      8,
			EGL_ALPHA_SIZE,     8,
			EGL_DEPTH_SIZE,     24,
			EGL_STENCIL_SIZE,   8,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_NONE
		};

		EGLint numConfigs;
		EGLint majorVersion;
		EGLint minorVersion;
		EGLConfig config;
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_CONTEXT_MINOR_VERSION_KHR,1,EGL_NONE };

		// Get Display
		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if (display == EGL_NO_DISPLAY)
		{
			display = eglGetDisplay(GetDC(hwnd));
			if (display == EGL_NO_DISPLAY)
				return EGL_FALSE;
		}

		// Initialize EGL
		if (!eglInitialize(display, &majorVersion, &minorVersion))
		{
			return EGL_FALSE;
		}

		// Get configs
		if (!eglGetConfigs(display, NULL, 0, &numConfigs))
		{
			return EGL_FALSE;
		}

		// Choose config
		if (!eglChooseConfig(display, attribList, &config, 1, &numConfigs))
		{
			return EGL_FALSE;
		}

		// Create a surface
		surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hwnd, NULL);
		if (surface == EGL_NO_SURFACE)
		{
			return EGL_FALSE;
		}

		// Create a GL context
		context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);

		if (context == EGL_NO_CONTEXT)
		{
			return EGL_FALSE;
		}

		// Make the context current
		if (!eglMakeCurrent(display, surface, surface, context))
		{
			return EGL_FALSE;
		}
	}

};