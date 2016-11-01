#pragma once

#include "ResourceManager.h"
#include "EngineUtil.h"
#include "FreeImage.h"
#include <memory>


class Image{
public:
	string name;//file name
	string filepath;

	enum ImageFormat
	{
		IM_JPEG,
		IM_PNG,
		IM_TGA,
		IM_BMP,
		IM_HDR,
		IM_TIF,
		IM_UNKNOWN
	};

	enum ImagePixelFormat
	{
		UNSIGNED_BYTE,
		UNSIGNED_SHORT,
		UNSIGNED_INT,
		UNSIGNED_INT_24_8,
		UNSIGNED_CHAR_8_8_8,
		UNSIGNED_CHAR_8_8_8_8,
		UNSIGNED_CHAR_10_10_10_2,
		FLOAT_32,
		FLOAT_32_32,
		FLOAT_32_32_32,
		FLOAT_32_32_32_32,
		FLOAT_16_16_16_16,
		FLOAT_16_16_16,
		UNSIGNED_INT_16_16_16_16,
		FormatCount
	};
	struct FormatInfo
	{
		int ByteSize;
		FormatInfo(int x) { ByteSize = x; }
		FormatInfo() {}
	};
	static vector<FormatInfo> formatInfos;
	ImageFormat mImageformat = IM_UNKNOWN;

	ImagePixelFormat mPixelformat;

	unsigned int width=0, height=0;
	unsigned int bpp=0;
	vector<BYTE> pdata;
	//unique_ptr<unsigned char[]> pdata;
	//void * pdata=NULL;
	FIBITMAP * fib=NULL;
	bool bgra = false;
	Image() {};
	Image(string filepath);
	Image* LoadImage(string filepath, bool flip_vertical = false);
	Image* CreateImageFromData(ImagePixelFormat format,int width,int height,void* data,string name="");
	static void SaveRGBA32BitsToTif(void* pSrc, string filename, int w, int h);
	static void SaveRGBA8BitsToPng(void* pSrc, string filename, int w, int h);
	static void SaveRGB32BitsToHdr(void* pSrc, string filename, int w, int h);
	bool m_valid = false;
	void test();

	~Image();
private:
	void readDataBytes(FIBITMAP * fib, vector<BYTE>& dst);
	ImageFormat getImageFormat(FREE_IMAGE_FORMAT fm);
	void ConvertBetweenBGRandRGB(vector<BYTE>& input, int pixel_width,
		int pixel_height,int bytes_per_pixel);

};

class ImageHelper {

};





class ImageManager :public ResourceManager<Image*>{
public:
	static ImageManager& getInstance()
	{
		static ImageManager    instance;
		return instance;
	}

	Image* CreateImageFromFile(string filepath, bool filp_vertical=false);
	vector<Image*> CreateImageListFromFiles(vector<string> paths);
private:
	ImageManager(){
	}

	ImageManager(ImageManager const&);              // Don't Implement.
	void operator=(ImageManager const&); // Don't implement
};