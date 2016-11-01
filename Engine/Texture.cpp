#include "Texture.h"
#include "Image.h"



HW::Texture::Texture() :width(0), height(0), texture_type(Texture_2D)
{
	//texture_mag_filter = texture_min_filter = TextureFilter_Linear;
	//texture_warp_s = texture_warp_t = TextureWarp_Clamp;
	texture_sampler = SamplerManager::getInstance().get("RepeatLinearMipLinear");
	m_valid = false;
}

HW::Texture::~Texture()
{

}

int HW::Texture::CalcMipmapLevel(int width, int height)
{
	int res;
	if (width > height) res = width; else res = height;
	int level = 1;
	if (res > 1)
		level = floor(log2(res))+1;
	return level;
}

void HW::Texture::setFormatFromImage(Image* im)
{
	switch (im->mPixelformat)
	{
	case Image::UNSIGNED_CHAR_8_8_8:
		texture_internal_format = RGB8; break;
		
	case Image::UNSIGNED_CHAR_8_8_8_8:
		texture_internal_format = RGBA8; break;
	case Image::FLOAT_16_16_16:
		texture_internal_format = RGB16F; break;
	case Image::FLOAT_16_16_16_16:
		texture_internal_format = RGBA16F; break;
	case Image::FLOAT_32:
		texture_internal_format = R32F; break;
	case Image::FLOAT_32_32:
		texture_internal_format = RG32F; break;
	case Image::FLOAT_32_32_32:
		texture_internal_format = RGB32F; break;
	case Image::FLOAT_32_32_32_32:
		texture_internal_format = RGBA32F; break;
	default:
		break;
	}
	width = im->width;
	height = im->height;
}

void HW::Texture::setFormat(Texture::TextureFormat format)
{
	texture_internal_format = format;
}
