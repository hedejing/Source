#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "mathlib.h"
#include <vector>
#include <map>
#include "OPENGL_Include.h"
using namespace std;
using namespace HW;

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	int size_w;
	int size_h;    // Size of glyph
	int Bearing_w;
	int Bearing_h;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};


class RenderTextPipeline
{
public:
	RenderTextPipeline();
	~RenderTextPipeline();
	void Init();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, Vector3 color);
private:
	std::map<GLchar, Character> Characters;
	GLGpuProgram* program;
	GLuint VAO, VBO;
	int w, h;

};