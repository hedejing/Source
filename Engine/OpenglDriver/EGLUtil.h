#pragma once
#include "OPENGL_Include.h"
#include "RenderSystemEnum.h"
#include "EGLTexture.h"
#include <assert.h>
#include <stdlib.h>
#include "Logger.h"
using namespace HW;

#ifdef _DEBUG
#define GLCHECKERROR	
#endif // DEBUG
#ifdef GLCHECKERROR
#define GL_CHECK(x)                                                                             \
    x;                                                                                          \
    {                                                                                           \
        GLenum glError = glGetError();                                                          \
        if(glError != GL_NO_ERROR)                                                              \
        {                                                                                       \
            Logger::WriteLog("glGetError() = %i (%#.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
            exit(1);                                                                 \
        }                                                                                       \
    }
#else 
#define GL_CHECK(x)                                                                             \
    x;   
#endif
//#define GLErrorLog Logger::WriteLog("glGetError() = %i (%#.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);																								




class EGLUtil
{
public:
	EGLUtil() {}
	~EGLUtil() {}

	static GLenum Get(ComparisonFunc func)
	{
		switch (func)
		{
		case COMP_NEVER:
			return GL_NEVER;
		case COMP_LESS:
			return GL_LESS;
		case HW::COMP_EQUAL:
			return GL_EQUAL;
		case COMP_LESS_EQUAL:
			return GL_LEQUAL;
		case COMP_GREATER:
			return GL_GREATER;
		case COMP_NOT_EQUAL:
			return GL_NOTEQUAL;
		case COMP_GREATER_EQUAL:
			return GL_GEQUAL;
		case COMP_ALWAYS:
			return GL_ALWAYS;

		}
		assert(false && "never reach here");
		return 0;
	}
	static GLenum Get(BlendOperand bo)
	{
		switch (bo)
		{
		case BLEND_ZERO:
			return GL_ZERO;
		case BLEND_ONE:
			return GL_ONE;
		case BLEND_SRC_COLOR:
			return GL_SRC_COLOR;
		case BLEND_INV_SRC_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case BLEND_SRC_ALPHA:
			return GL_ALPHA;
		case BLEND_INV_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		case BLEND_DEST_ALPHA:
			return GL_DST_ALPHA;
		case BLEND_INV_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BLEND_DEST_COLOR:
			return GL_DST_COLOR;
		case BLEND_INV_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case BLEND_SRC_ALPHA_SAT:
			return GL_ZERO;
		case BLEND_BLEND_FACTOR:
			return GL_CONSTANT_COLOR;
		case BLEND_INV_BLEND_FACTOR:
			return GL_ONE_MINUS_CONSTANT_COLOR;
		}
		assert(false && "never reach here");
		return 0;
	}
	static GLenum Get(BlendOperation bo)
	{
		switch (bo)
		{
		case BLEND_OP_ADD:
			return GL_FUNC_ADD;
		case BLEND_OP_SUBTRACT:
			return GL_FUNC_SUBTRACT;
		case BLEND_OP_REV_SUBTRACT:
			return GL_FUNC_REVERSE_SUBTRACT;
		}
		assert(false && "never reach here");
		return 0;
	}
	static GLenum Get(StencilOperation so)
	{
		switch (so)
		{
		case STENCIL_OP_KEEP:
			return GL_KEEP;
		case STENCIL_OP_ZERO:
			return GL_ZERO;
		case STENCIL_OP_REPLACE:
			return GL_REPLACE;
		case STENCIL_OP_INCR_SAT:
			return GL_INCR_WRAP;
		case STENCIL_OP_DECR_SAT:
			return GL_DECR_WRAP;
		case STENCIL_OP_INVERT:
			return GL_INVERT;
		case STENCIL_OP_INCR:
			return GL_INCR;
		case STENCIL_OP_DECR:
			return GL_DECR;
		}
		assert(false && "never reach here");
		return 0;
	}
	//static GLenum Get(Texture::TextureWarp warp)
	//{
	//	switch (warp)
	//	{
	//	case HW::Texture::TextureWarp_Clamp:
	//		return GL_CLAMP_TO_EDGE;
	//	case HW::Texture::TextureWarp_Repeat:
	//		return GL_REPEAT;
	//	case HW::Texture::TextureWarp_Mirror:
	//		return GL_MIRRORED_REPEAT;
	//	}
	//}
	static GLenum Get(Sampler::SamplerWrap warp)
	{
		switch (warp)
		{
		case Sampler::TextureWrap_Clamp:
			return GL_CLAMP_TO_EDGE;
		case Sampler::TextureWrap_Repeat:
			return GL_REPEAT;
		case Sampler::TextureWrap_Mirror:
			return GL_MIRRORED_REPEAT;
		}
	}
	//static GLenum Get(Texture::TextureFilter filter)
	//{
	//	switch (filter)
	//	{
	//	case HW::Texture::TextureFilter_Nearest:
	//		return GL_NEAREST;
	//	case HW::Texture::TextureFilter_Linear:
	//		return GL_LINEAR;
	//	default:
	//		return GL_LINEAR;
	//	}
	//}
	static GLenum Get(Sampler::TextureFilter filter)
	{
		switch (filter)
		{
		case Sampler::TextureFilter_Nearest:
			return GL_NEAREST;
		case Sampler::TextureFilter_Linear:
			return GL_LINEAR;
		case Sampler::TextureFilter_Nearest_Mipmap_Nearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case Sampler::TextureFilter_Linear_Mipmap_Nearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case Sampler::TextureFilter_Nearest_Mipmap_Linear:
			return GL_NEAREST_MIPMAP_LINEAR;
		case Sampler::TextureFilter_Linear_Mipmap_Linear:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return GL_LINEAR;
		}
	}
	static GLenum Get(Texture::TextureType texture_type)
	{
		switch (texture_type)
		{
		case HW::Texture::Texture_2D:
			return GL_TEXTURE_2D;
		case HW::Texture::Texture_3D:
			return GL_TEXTURE_3D;
		case HW::Texture::Texture_Cube:
			return GL_TEXTURE_CUBE_MAP;
		default:
			return GL_TEXTURE_2D;
		}
	}

	static GLenum Get(Texture::TextureFormat pixelformat)
	{
		switch (pixelformat)
		{
		case HW::Texture::LUMINANCE:
			return GL_LUMINANCE;
		case HW::Texture::RGBA8:
			return GL_RGBA;
		case HW::Texture::RGB8:
			return GL_RGB;
		case HW::Texture::DEPTH16:
			return GL_DEPTH_COMPONENT16;
		case HW::Texture::DEPTH32:
			return GL_DEPTH_COMPONENT32F;
		case HW::Texture::RGBA32F:
			return GL_RGBA32F;
		default:
			return GL_RGBA;
		}
	}
	//static GLenum Get(Texture::PixelData pixeldata)
	//{
	//	switch (pixeldata)
	//	{
	//	case HW::Texture::UNSIGNED_CHAR:
	//		return GL_UNSIGNED_BYTE;
	//	case HW::Texture::UNSIGNED_SHORT:
	//		return GL_UNSIGNED_SHORT;
	//	case HW::Texture::FLOAT:
	//		return GL_FLOAT;
	//	default:
	//		return GL_UNSIGNED_BYTE;
	//	}
	//}

	static GLenum Get(BUFFER_STORE buffer_store)
	{
		switch (buffer_store)
		{
		case HW::BS_STATIC:
			return GL_STATIC_DRAW;
			break;
		case HW::BS_DYNAMIC:
			return GL_DYNAMIC_DRAW;
			break;
		case HW::BS_STREAM:
			return GL_STREAM_DRAW;
			break;
		default:
			return GL_DYNAMIC_DRAW;
			break;
		}
	}
};

