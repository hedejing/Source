#include "EGLTexture.h"
#include "Image.h"



EGLTexture::EGLTexture() :Texture()
{
	texture_internal_format = Texture::RGBA8;
}

void EGLTexture::createInternalRes()
{
	if (m_valid)
		return;

	setFormat(texture_internal_format);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_id);

	texture_target = EGLUtil::Get(texture_type);
	if (MSAASampleNum > 1) 
		texture_target = GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(texture_target, texture_id);

	switch (mipmap_mode)
	{
	case Texture::TextureMipmap_Auto:
	case Texture::TextureMipmap_Manual:
		mipmap_level = CalcMipmapLevel(width, height);
		break;
	case Texture::TextureMipmap_None:
		mipmap_level = 1;
		break;
	default:
		break;
	}
	m_gl_texture_format_internal = getGLInternalFormat(texture_internal_format);
	if (texture_type == Texture_Cube) {
		createTextureCubeMap();
	}
	else if (texture_type == Texture_2D)
	{
		if (image != NULL&&image->pdata.size()>0) 
			createTexture2DImage();
		else
			createTextureEmpty();
	}

	GL_CHECK();
	m_valid = true;
}

void EGLTexture::useTexture()
{
	if (!m_valid)
		createInternalRes();
	
	glBindTexture(texture_target, texture_id);
}

void EGLTexture::releaseInternalRes()
{
	if (m_valid)
	{
		glDeleteTextures(1, &texture_id);
		texture_id = 0;
		m_valid = false;
	}
}

GLenum EGLTexture::getGLInternalFormat(Texture::TextureFormat format)
{
	GLenum gl_texture_format_internal = GL_RGBA8;
	switch (format)
	{
	case Texture::RGBA8:
		gl_texture_format_internal = GL_RGBA8;
		break;
	case Texture::DEPTH32:
		gl_texture_format_internal = GL_DEPTH_COMPONENT32F;
		break;
	case Texture::DEPTH:
		gl_texture_format_internal = GL_DEPTH_COMPONENT;
		break;
	case Texture::DEPTH16:
		gl_texture_format_internal = GL_DEPTH_COMPONENT16;
		break;
	case Texture::DEPTH24:
		gl_texture_format_internal = GL_DEPTH_COMPONENT24;
		break;
	case Texture::DEPTH24_STENCIL8:
		gl_texture_format_internal = GL_DEPTH24_STENCIL8;
		break;
	case Texture::RGBA32F:
		gl_texture_format_internal = GL_RGBA32F;
		break;
	case Texture::RG32UI:
		gl_texture_format_internal = GL_RG32UI;
		break;
	case Texture::RGBA16F:
		gl_texture_format_internal = GL_RGBA16F;
		break;
	case Texture::RGB16F:
		gl_texture_format_internal = GL_RGB16F;
		break;
	case Texture::RGBA16UI:
		gl_texture_format_internal = GL_RGBA16UI;
		break;
	case Texture::LUMINANCE:
		gl_texture_format_internal = GL_LUMINANCE;
		break;
	case Texture::RGB8:
		gl_texture_format_internal = GL_RGB8;
		break;
	case Texture::RGB10_A2:
		gl_texture_format_internal = GL_RGB10_A2;
		break;
	case Texture::R32F:
		gl_texture_format_internal = GL_R32F;
	case Texture::RG32F:
		gl_texture_format_internal = GL_RG32F;
		break;
	}
	return gl_texture_format_internal;
}

void EGLTexture::GenerateMipmap()
{
	if (m_valid){
		glBindTexture(texture_target, texture_id);
		glGenerateMipmap(texture_target);
	}
	GL_CHECK();
}

void EGLTexture::getGLImageFormat(Image* im, GLenum& imageformat, GLenum&pixelformat)
{
	switch (im->mPixelformat)
	{
	case Image::UNSIGNED_CHAR_8_8_8:
	{
									   imageformat = GL_RGB;
									   pixelformat = GL_UNSIGNED_BYTE;
									   break;
	}
	case Image::UNSIGNED_CHAR_8_8_8_8:
	{
										 imageformat = GL_RGBA;
										 pixelformat = GL_UNSIGNED_BYTE;
										 break;
	}
	case Image::UNSIGNED_CHAR_10_10_10_2:
	{
											imageformat = GL_RGBA;
											pixelformat = GL_UNSIGNED_INT_2_10_10_10_REV;
											break;
	}
	case Image::FLOAT_32:
	{
							imageformat = GL_RED;
							pixelformat = GL_FLOAT;
							break;
	}
	case Image::FLOAT_32_32:
	{
		imageformat = GL_RG;
		pixelformat = GL_FLOAT;
		break;
	}
	case Image::FLOAT_32_32_32:
	{
								  imageformat = GL_RGB;
								  pixelformat = GL_FLOAT;
								  break;
	}
	case Image::FLOAT_32_32_32_32:
	{
									 imageformat = GL_RGBA;
									 pixelformat = GL_FLOAT;
									 break;
	}
	case Image::FLOAT_16_16_16_16:
	{
									 imageformat = GL_RGBA;
									 pixelformat = GL_HALF_FLOAT;
									 break;
	}
	case Image::FLOAT_16_16_16:
	{
								  imageformat = GL_RGB;
								  pixelformat = GL_HALF_FLOAT;
								  break;
	}
	case Image::UNSIGNED_INT_16_16_16_16:
	{
											imageformat = GL_RGBA;
											pixelformat = GL_UNSIGNED_SHORT;
											break;
	}
	default:
		assert(false && "not support image format");
		break;
	}
}

void EGLTexture::createTexture2DImage()
{
	glTexStorage2D(texture_target, mipmap_level, m_gl_texture_format_internal, width, height);
	GLenum gl_texture_format, gl_pixel_format;
	getGLImageFormat(image, gl_texture_format, gl_pixel_format);
	glTexSubImage2D(texture_target, 0, 0, 0, image->width, image->height, gl_texture_format, gl_pixel_format, &image->pdata[0]);
	if (mipmap_mode == TextureMipmap_Auto)
		glGenerateMipmap(texture_target);

	
}

void EGLTexture::createTextureEmpty()
{
#ifdef ANDROID
	mipmap_level = 1;
#endif // ANDROID

	if (MSAASampleNum == 1)
		glTexStorage2D(texture_target, mipmap_level, m_gl_texture_format_internal, width, height);
	else
#ifndef ANDROID
		glTexImage2DMultisample(texture_target, MSAASampleNum, m_gl_texture_format_internal, width, height, GL_TRUE);
#endif
	;
		//glTexStorage2DMultisample(texture_target, MSAASampleNum, m_gl_texture_format_internal, width, height, GL_TRUE);
}

void EGLTexture::createTextureCubeMap()
{
	
	glTexStorage2D(texture_target, mipmap_level, m_gl_texture_format_internal, width, height);
	if (image_cube.size() != 6) return;
	for (int i = 0; i < 6; i++){
		Image* image = image_cube[i];
		GLenum gl_texture_format, gl_pixel_format;
		getGLImageFormat(image, gl_texture_format, gl_pixel_format);
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, image->width, image->height, gl_texture_format, gl_pixel_format, &image->pdata[0]);
		GL_CHECK();
	}
	
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	if (mipmap_mode == TextureMipmap_Auto) 
		glGenerateMipmap(texture_target);
	GL_CHECK();
}

void EGLTexture::SaveTextureToImage(string name, SaveType t)
{
#ifdef USE_OPENGL_CONTEXT
	if (t== SaveType::ST_FLOAT) {
		int size = width*height * 4 * sizeof(float);
		vector<float> bits(size, 0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &bits[0]);
		////glGetTextureImageEXT(texture_id, 0, GL_RGBA, GL_FLOAT, size, &bits[0]);
		//GL_CHECK();
		//Image::SaveRGBA32BitsToTif(&bits[0], name, width, height);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &bits[0]);
		GL_CHECK();
		Image::SaveRGB32BitsToHdr(&bits[0], name, width, height);
	}
	if (t == SaveType::ST_UNSIGNED_BYTE) {
		int size = width*height * 4 ;
		vector<BYTE> bits(size, 0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, &bits[0]);
		//glGetTextureImageEXT(texture_id, 0, GL_RGBA, GL_FLOAT, size, &bits[0]);
		GL_CHECK();
		Image::SaveRGBA8BitsToPng(&bits[0], name, width, height);
	}
	if (t == SaveType::ST_DEPTH) {
		//int size = width*height;
		//vector<float> bits(size, 0);
		//glBindTexture(GL_TEXTURE_2D, texture_id);
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &bits[0]);
		////glGetTextureImageEXT(texture_id, 0, GL_RGBA, GL_FLOAT, size, &bits[0]);
		//GL_CHECK();
		//Image::SaveRGBA8BitsToPng(&bits[0], name, width, height);
	}
	if (t == SaveType::ST_Stencil) {
	}
#endif
}

void EGLTexture::BlitMSAATexture(Texture* in, Texture* out)
{
	//out->createInternalRes();
	//EGLTexture* msaaTex = (EGLTexture*)in;
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glCopyTexSubImage2D()
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFBO);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

EGLTexture::~EGLTexture()
{
	releaseInternalRes();
}
