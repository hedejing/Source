#pragma once
#include "ContextChoose.h"

#ifdef USE_OPENGL_ES_CONTEXT
#include <egl/egl.h>
#include <egl/eglext.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#endif // GLES


#ifdef USE_OPENGL_CONTEXT
#include <GL/glew.h>
#endif 