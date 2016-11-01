#include "Image.h"
#include <array>


vector<Image::FormatInfo> SetUp() {
	vector<Image::FormatInfo> formatInfos;
	formatInfos.resize(Image::FormatCount);
	formatInfos[Image::FLOAT_32_32_32] = Image::FormatInfo(12);
	formatInfos[Image::FLOAT_32_32_32_32] = Image::FormatInfo(16);
	return formatInfos;
}



vector<Image::FormatInfo> Image::formatInfos= SetUp();

Image::Image(string filepath)
{
	LoadImage(filepath);
}

Image* Image::LoadImage(string _filepath,bool flip_vertical)
{
	string fullpath = FileUtil::getSceneFileValidPath(_filepath);
	if (fullpath.empty()) return NULL;
	string purefilename = FileUtil::getPureFileName(fullpath);
	name = purefilename;
	this->filepath = fullpath;

	FREE_IMAGE_FORMAT fileFormat = FIF_UNKNOWN;
	fileFormat = FreeImage_GetFileType(fullpath.c_str());
	if (fileFormat == FIF_UNKNOWN)
	{
		fileFormat = FreeImage_GetFIFFromFilename(fullpath.c_str());
	}
	// make sure image can be reading.
	assert(FreeImage_FIFSupportsReading(fileFormat));
	mImageformat=getImageFormat(fileFormat);
	switch (fileFormat){
	case FIF_BMP:
	case FIF_JPEG:
	case FIF_PNG:
	case FIF_TARGA:
	case FIF_HDR:
	case FIF_DDS:
	case FIF_TIFF:
	{
					fib = FreeImage_Load(fileFormat, _filepath.c_str());
					assert(fib && " FreeImage_Load function failed.");
					width = FreeImage_GetWidth(fib);
					height = FreeImage_GetHeight(fib);
					FREE_IMAGE_TYPE imagetype = FreeImage_GetImageType(fib);
					FREE_IMAGE_COLOR_TYPE colortype = FreeImage_GetColorType(fib);
					bpp = FreeImage_GetBPP(fib);
					if (flip_vertical)
						FreeImage_FlipVertical(fib);
					switch (imagetype)
					{
					case FIT_UNKNOWN:
					case FIT_COMPLEX:
					case FIT_UINT32:
					case FIT_INT32:
					case FIT_DOUBLE:
					case FIT_UINT16:
					case FIT_INT16:
					case FIT_FLOAT:
					default:
						assert(false && "image type not support.");
						return NULL;
						break;
					case  FIT_RGBF: {
						mPixelformat = FLOAT_32_32_32;
						readDataBytes(fib, pdata);
						break;
					}
					case FIT_RGBAF: {
						mPixelformat = FLOAT_32_32_32_32;
						readDataBytes(fib, pdata);
						break;
					}
					case FIT_BITMAP:
					{

									   if (colortype == FIC_MINISWHITE || colortype == FIC_MINISBLACK)
									   {
										   FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fib);
										   // free old bitmap and replace
										   FreeImage_Unload(fib);
										   fib = newBitmap;
										   // get new formats
										   bpp = FreeImage_GetBPP(fib);
										   colortype = FreeImage_GetColorType(fib);
										   
									   }
									   else if (bpp < 8 || colortype == FIC_PALETTE || colortype == FIC_CMYK)
									   {
										   FIBITMAP* newBitmap = FreeImage_ConvertTo32Bits(fib);
										
										   // free old bitmap and replace
										   FreeImage_Unload(fib);
										   fib = newBitmap;
										   // get new formats
										   bpp = FreeImage_GetBPP(fib);
										   colortype = FreeImage_GetColorType(fib);
										   
									   }
									   readDataBytes(fib, pdata);
									   switch (bpp)
									   {
									   case 24:{
												   mPixelformat = UNSIGNED_CHAR_8_8_8;
												   ConvertBetweenBGRandRGB(pdata, width, height,3);
												   break;
									   }
									   case 32:{
												   mPixelformat = UNSIGNED_CHAR_8_8_8_8;
												   ConvertBetweenBGRandRGB(pdata, width, height, 4);
												   break;
									   }
									   }
									   

					}break;
					case FIT_RGB16:
					{
									  FIBITMAP * newfib = FreeImage_ConvertToType(fib, FIT_BITMAP);
									  FreeImage_Unload(fib);
									  fib = newfib;
									  bpp = FreeImage_GetBPP(fib);
									  readDataBytes(fib, pdata);
									  mPixelformat = UNSIGNED_CHAR_8_8_8;
									  readDataBytes(fib, pdata);
									  
									  
					}break;
					case FIT_RGBA16:
					{
									   FIBITMAP * newfib = FreeImage_ConvertToType(fib, FIT_BITMAP);
									   FreeImage_Unload(fib);
									   fib = newfib;
									   bpp = FreeImage_GetBPP(fib);
									   mPixelformat = UNSIGNED_CHAR_8_8_8_8;
									   readDataBytes(fib, pdata);
									   
					}break;
					}

					
					FreeImage_Unload(fib); fib = NULL;
					
					break;
	}
	default:
	{
			   assert(false && "image not support");
			   break;
	}
	}
	m_valid = true;
	return this;
}

void Image::test()
{
	//Image* im1 = LoadImage("default_asserts/3.jpg");
	//Image* im2 = LoadImage("default_asserts/2.tga");
	//Image* im3 = LoadImage("default_asserts/4.jpg");
	//Image* im4 = LoadImage("default_asserts/noisy-texture.png");
	//Image* im5 = LoadImage("default_asserts/ennis_c02.hdr");

	//int w = 300, h = 400;
	//vector<float> b(w*h * 4,15.0);
	//SaveRGBA32BitsToTif(&b[0], "f.tif", w, h);
	//Image* im1 = LoadImage("15.tif");
}

Image::~Image()
{
	if (fib != NULL)
		FreeImage_Unload(fib);
}

void Image::readDataBytes(FIBITMAP * fib, vector<BYTE>& dst)
{
	unsigned char * srcData = FreeImage_GetBits(fib);
	//dst = srcData;
	//return;
	unsigned int srcPitch = FreeImage_GetPitch(fib);

	unsigned int dstPitch = width * (bpp / 8);

	dst.resize(height*dstPitch);
	BYTE * pSrc;
	BYTE * pDst = (BYTE *)&dst[0];

	for (size_t y = 0; y < height; y++)
	{
		pSrc = srcData + (y)* srcPitch;
		memcpy(pDst, pSrc, dstPitch);
		pDst += dstPitch;
	}
}

Image::ImageFormat Image::getImageFormat(FREE_IMAGE_FORMAT fm)
{
	ImageFormat f;
	switch (fm)
	{
	case FIF_BMP:
		f = IM_BMP; break;
	case FIF_JPEG:
		f = IM_JPEG; break;
	case FIF_PNG:
		f = IM_PNG; break;
	case FIF_TARGA:
		f = IM_TGA; break;
	case FIF_HDR:
		f = IM_HDR; break;
	default:
		f = IM_UNKNOWN; break;
		break;
	}
	return f;
}

void Image::ConvertBetweenBGRandRGB(vector<BYTE>& input, int pixel_width, int pixel_height,int bytes_per_pixel =3)
{
	int offset = 0;

	for (int y = 0; y < pixel_height; y++) {
		for (int x = 0; x < pixel_width; x++) {
			auto tmp = input[offset];
			input[offset] = input[offset + 2];
			input[offset + 2] = tmp;
			offset += bytes_per_pixel;
		}
	}
}

Image* Image::CreateImageFromData(ImagePixelFormat format, int width, int height, void* data, string name/*=""*/)
{
	this->pdata.clear();
	int size = width*height;
	int bytenum = formatInfos[format].ByteSize;
	size *= bytenum;
	this->pdata.resize(size);
	memcpy(&pdata[0], data, size);
	//this->pdata = data;
	this->width = width;
	this->height = height;
	this->mPixelformat = format;
	return this;
}

void Image::SaveRGBA32BitsToTif(void* pSrc, string filename,int w, int h)
{
	FIBITMAP *image = FreeImage_AllocateT(FIT_RGBAF, w, h);
	int pitch=FreeImage_GetPitch(image);
	int bpp= FreeImage_GetBPP(image);
	BYTE* pDst = FreeImage_GetBits(image);
	memcpy(pDst, pSrc, pitch*h);


	auto fullname = filename + ".tif";
	if (FreeImage_Save(FIF_TIFF, image, fullname.c_str()))
		printf("\nImage %s saved successfully\n\n", fullname.c_str());
	else
		printf("\n***IMAGE NOT SAVED***\n\n");
	//for (int y = 0; y < h; y++) {
	//	FIRGBAF *bits = (FIRGBAF *)FreeImage_GetScanLine(image, y);
	//	for (int x = 0; x < w; x++) {
	//		bits[x].red = 7658;
	//		bits[x].green = 128;
	//		bits[x].blue = 128;
	//		bits[x].alpha = 128;
	//	}
	
	//FIBITMAP * bitmap = FreeImage_Allocate(w, h,32);
	//
	//RGBQUAD color;
	//color.rgbBlue = 100;
	//color.rgbGreen = 0;
	//color.rgbRed = 0;

	//bpp = FreeImage_GetBPP(bitmap);
	//for (int y = 0; y < FreeImage_GetHeight(bitmap); y++) {
	//	BYTE *bits = (BYTE *)FreeImage_GetScanLine(bitmap, y);
	//	
	//	for (int x = 0; x < FreeImage_GetWidth(bitmap); x++) {
	//		//FreeImage_SetPixelColor(bitmap, x, y, &color);
	//		bits[4*x] = 128;
	//		bits[4*x + 1] = 128;
	//		//bits[x + 2] = 128;
	//		//bits[x + 3] = 128;
	//	}
	//}
	//bool b=FreeImage_Save(FIF_PNG, bitmap, "16.png", 0);

}

void Image::SaveRGBA8BitsToPng(void* pSrc, string filename, int w, int h)
{
	FIBITMAP * image = FreeImage_Allocate(w, h,32);
	int pitch = FreeImage_GetPitch(image);
	int bpp = FreeImage_GetBPP(image);
	BYTE* pDst = FreeImage_GetBits(image);
	memcpy(pDst, pSrc, pitch*h);

	auto fullname = filename + ".png";
	if (FreeImage_Save(FIF_PNG, image, fullname.c_str()))
		printf("\nImage %s saved successfully\n\n", fullname.c_str());
	else
		printf("\n***IMAGE NOT SAVED***\n\n");
}

void Image::SaveRGB32BitsToHdr(void* pSrc, string filename, int w, int h)
{
	FIBITMAP *image = FreeImage_AllocateT(FIT_RGBF, w, h);
	int pitch = FreeImage_GetPitch(image);
	int bpp = FreeImage_GetBPP(image);
	BYTE* pDst = FreeImage_GetBits(image);
	memcpy(pDst, pSrc, pitch*h);


	auto fullname = filename + ".hdr";
	if (FreeImage_Save(FIF_HDR, image, fullname.c_str()))
		printf("\nImage %s saved successfully\n\n", fullname.c_str());
	else
		printf("\n***IMAGE NOT SAVED***\n\n");
}

Image* ImageManager::CreateImageFromFile(string filepath, bool filp_vertical)
{
	string fullpath = FileUtil::getSceneFileValidPath(filepath);
	if (fullpath.empty()) return NULL;
	string purefilename = FileUtil::getPureFileName(fullpath);

	Image* im = get(purefilename);//以filename作为关键字查找
	if (im == NULL)
	{
		im = new Image;
		im->LoadImage(fullpath, filp_vertical);
		add(im);
	}
	return im;
}

vector<Image*> ImageManager::CreateImageListFromFiles(vector<string> paths)
{
	vector<Image*> images;
	for (auto path : paths){
		Image* im = CreateImageFromFile(path);
		assert(im != NULL);
		images.push_back(im);
	}
	return images;
}
