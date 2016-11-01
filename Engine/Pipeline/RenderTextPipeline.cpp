#include "RenderTextPipeline.h"
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ResourceManager.h"

#include "gl_program.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "EGLUtil.h"
#include "RenderTarget.h"
RenderTextPipeline::~RenderTextPipeline()
{

}

void RenderTextPipeline::Init()
{
#ifdef ANDROID
	return;
#endif // ANDROID

	//auto& grm = GlobalResourceManager::getInstance();
	//auto& rt_mgr = RenderTargetManager::getInstance();
	//auto mainwindow = rt_mgr.get("MainWindow");
	//w = mainwindow->width;
	//h = mainwindow->height;
	//program = new GLGpuProgram();

	//string vsName = "rendertextvs";
	//string fsName = "rendertextfs";
	//string vsPath = "RenderText/RenderText.vs";
	//string fsPath = "RenderText/RenderText.fs";

	//program->attachShader(ShaderType::ST_VERTEX, vsPath, vsName);

	//program->attachShader(ShaderType::ST_PIXEL, fsPath, fsName);

	//program->CompileAndLink();
	//Camera camera;

	////people
	//camera.setFrustum(-w/2, w/2, -h/2,h/2, 1, 3);
	//camera.setProjectionType(Camera::PT_ORTHOGONAL);
	//Matrix4 projection = camera.ortho(0, w, 0, h);
	////glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
	//program->use();
	//glUniformMatrix4fv(glGetUniformLocation(program->programID, "projection"), 1, GL_FALSE, &(projection.transpose()[0][0]));

	//// FreeType
	//FT_Library ft;
	//// All functions return a value different than 0 whenever an error occurred
	//if (FT_Init_FreeType(&ft))
	//	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	//// Load font as face
	//FT_Face face;
	//if (FT_New_Face(ft, "Arial.ttf", 0, &face))
	//	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	//// Set size to load glyphs as
	//FT_Set_Pixel_Sizes(face, 0, 48);

	//// Disable byte-alignment restriction
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//// Load first 128 characters of ASCII set
	//for (GLubyte c = 0; c < 128; c++)
	//{
	//	// Load character glyph 
	//	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	//	{
	//		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	//		continue;
	//	}
	//	// Generate texture
	//	GLuint texture;
	//	glGenTextures(1, &texture);
	//	glBindTexture(GL_TEXTURE_2D, texture);
	//	GL_CHECK();
	//	glTexImage2D(
	//		GL_TEXTURE_2D,
	//		0,
	//		GL_R8,
	//		face->glyph->bitmap.width,
	//		face->glyph->bitmap.rows,
	//		0,
	//		GL_RED,
	//		GL_UNSIGNED_BYTE,
	//		face->glyph->bitmap.buffer
	//		);
	//	GL_CHECK();
	//	// Set texture options
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	// Now store character for later use
	//	Character character = {
	//		texture,
	//		int(face->glyph->bitmap.width), int(face->glyph->bitmap.rows),
	//		int(face->glyph->bitmap_left), int(face->glyph->bitmap_top),
	//		GLuint(face->glyph->advance.x)
	//	};
	//	Characters.insert(std::pair<GLchar, Character>(c, character));
	//}
	//glBindTexture(GL_TEXTURE_2D, 0);
	//// Destroy FreeType once we're finished
	//FT_Done_Face(face);
	//FT_Done_FreeType(ft);

	//
	//// Configure VAO/VBO for texture quads
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
	//GL_CHECK();
}

void RenderTextPipeline::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, Vector3 color)
{
#ifdef ANDROID
	return;
#endif // ANDROID
	// Activate corresponding render state	
	//program->use();
	//glUniform3f(glGetUniformLocation(program->programID, "textColor"), color.x, color.y, color.z);
	//glActiveTexture(GL_TEXTURE0);
	//glBindVertexArray(VAO);
	////glBindFramebuffer();
	//// Iterate through all characters
	//std::string::const_iterator c;
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//for (c = text.begin(); c != text.end(); c++)
	//{
	//	Character ch = Characters[*c];

	//	GLfloat xpos = x + ch.Bearing_w * scale;
	//	GLfloat ypos = y - (ch.size_h - ch.Bearing_h) * scale;

	//	GLfloat w = ch.size_w * scale;
	//	GLfloat h = ch.size_h * scale;
	//	// Update VBO for each character
	//	GLfloat vertices[6][4] = {
	//		{ xpos,     ypos + h,   0.0, 0.0 },
	//		{ xpos,     ypos,       0.0, 1.0 },
	//		{ xpos + w, ypos,       1.0, 1.0 },

	//		{ xpos,     ypos + h,   0.0, 0.0 },
	//		{ xpos + w, ypos,       1.0, 1.0 },
	//		{ xpos + w, ypos + h,   1.0, 0.0 }
	//	};
	//	// Render glyph texture over quad
	//	glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	//	// Update content of VBO memory
	//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	// Render quad
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
	//	x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	//}
	//glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_BLEND);
}

RenderTextPipeline::RenderTextPipeline()
{

}
