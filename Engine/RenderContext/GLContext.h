#pragma once
#pragma comment(lib,"glew32.lib")

#pragma comment(lib,"opengl32.lib")
#include <GL/glew.h>

class GLContext {
public:
	void Init() {
		glewInit();
	}
};