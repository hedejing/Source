#pragma once

#include "OPENGL_Include.h"

#include "Texture.h"
#include "EGLUtil.h"
#include "Logger.h"

class EGLTexture : public Texture
{
public:
	GLuint texture_id = -1;

	GLenum m_gl_texture_format_internal = 0;

	GLenum texture_target = 0;

	EGLTexture();


	virtual void createInternalRes();

	virtual void useTexture();

	virtual void GenerateMipmap();

	virtual void releaseInternalRes();


	virtual GLenum getGLInternalFormat(Texture::TextureFormat format);

	void getGLImageFormat(Image* im, GLenum& imageformat, GLenum&pixelformat);
	void createTexture2DImage();
	void createTextureEmpty();
	void createTextureCubeMap();
	virtual void SaveTextureToImage(string name, SaveType t);
	virtual void BlitMSAATexture(Texture* in, Texture* out);

	virtual ~EGLTexture();


};

class EGLTextureFactory :public TextureFactory
{
public:
	virtual Texture * create()
	{
		return new EGLTexture();
	}
};

