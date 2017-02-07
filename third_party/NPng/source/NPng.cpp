/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "NPng.h"
#include <stdio.h>
#include <assert.h>
#include <png.h>
#include <string>

using namespace std;

// png file signature size
#define PNGSIGSIZE 8
#define NPNG_ERROR(s) fprintf(stderr, "NPng Error: %s",  s)

// custom read function for loading png files from memory
void read_mem_data(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
	unsigned char **dataPtr = (unsigned char**)png_get_io_ptr(png_ptr);

	if(*dataPtr == NULL)
		return;

	memcpy(outBytes, (*dataPtr), byteCountToRead);

	(*dataPtr) += byteCountToRead;
}

NPng::NPng() : w(0), h(0), pixels(NULL)
{

}

NPng::NPng(const char *filename) : w(0), h(0), pixels(NULL)
{
	int ret = LoadPNG(filename);
	assert(ret == 0);
}

NPng::NPng(FILE *file, long long length) : w(0), h(0), pixels(NULL)
{
	int ret = LoadFromFile(file, length);
	assert(ret == 0);
}

NPng::NPng(unsigned char *data, long long length) : w(0), h(0), pixels(NULL)
{
	int ret = LoadFromMemory(data, length);
	assert(ret == 0);
}

NPng::~NPng()
{
	if(pixels != NULL)
		delete [] pixels;
}

unsigned int NPng::GetWidth()
{
	return w;
}

unsigned int NPng::GetHeight()
{
	return h;
}

unsigned char *NPng::GetPixels()
{
	return pixels;
}

void InitPngSettings(png_struct *pPngStruct, png_info *pPngInfo)
{
	unsigned int bitDepth = png_get_bit_depth(pPngStruct, pPngInfo); // bits per channel
	unsigned int channels = png_get_channels(pPngStruct, pPngInfo); // num channels
	unsigned int colorType = png_get_color_type(pPngStruct, pPngInfo); // (RGB, RGBA, Luminance, luminance alpha... palette... etc)

	if(bitDepth == 16)
		png_set_strip_16(pPngStruct); // convert down to to 8 bits per channel

	if(colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(pPngStruct); // expand grayscale
	
	if(png_get_valid(pPngStruct, pPngInfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(pPngStruct); // expand to RGBA
	
	png_set_filler(pPngStruct, 0xff, PNG_FILLER_AFTER); // fill last byte of 24bit images
	png_read_update_info(pPngStruct, pPngInfo); // required for palette alterations
}

bool NPng::LoadPNG(const char *filename)
{
	bool ret = false;

	FILE *pngFile = fopen(filename, "rb");
	
	if(pngFile)
	{
		ret = LoadFromFile(pngFile, 0);
		fclose(pngFile);
	}

	return ret;
}

bool NPng::LoadFromFile(FILE *file, long long length)
{
	Close();
	
	if(!file)
	{
		NPNG_ERROR("FILE ptr is NULL");
		return false;
	}

	png_struct *pPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pPngStruct)
	{
		NPNG_ERROR("failed to initialize libpng.");
		return false;
	}

	png_info *pPngInfo = png_create_info_struct(pPngStruct);
	if(!pPngInfo)
	{
		NPNG_ERROR("failed to initialize libpng.");
		png_destroy_read_struct(&pPngStruct, NULL, NULL);
		return false;
	}

// start reading the file

	unsigned char **rowPtrs = NULL;
	unsigned char header[PNGSIGSIZE];

	fread(header, 1, PNGSIGSIZE, file);
	if(png_sig_cmp(header, 0, PNGSIGSIZE) != 0)
	{
		NPNG_ERROR("couldn't read png header.");
		return false;
	}

	bool ret = false;

	if(setjmp(png_jmpbuf(pPngStruct)) == 0)
	{
		png_init_io(pPngStruct, file);
		png_set_sig_bytes(pPngStruct, PNGSIGSIZE);
		
		png_read_info(pPngStruct, pPngInfo);
		w = png_get_image_width(pPngStruct, pPngInfo);
		h = png_get_image_height(pPngStruct, pPngInfo);

		InitPngSettings(pPngStruct, pPngInfo);

		pixels = new unsigned char[w * h * 4];
		rowPtrs = new unsigned char*[h];
		
		for(unsigned int y = 0; y < h; y++)
		{
			rowPtrs[y] = pixels + (y * w * 4);
		}
		
		png_read_image(pPngStruct, rowPtrs);
		
		ret = true;
	}
	else // cleanup on error
	{
		NPNG_ERROR("Failed to load png file.");

		if(pixels != NULL) delete [] pixels;
		
		pixels = NULL;
		w = 0;
		h = 0;
		ret = false;
	}

	if(pPngStruct) png_destroy_read_struct(&pPngStruct, &pPngInfo, NULL);
	if(rowPtrs != NULL) delete [] rowPtrs;
	
	return ret;
}

bool NPng::LoadFromMemory(unsigned char *data, long long length)
{
	Close();
	
	if(!data)
	{
		NPNG_ERROR("data ptr is NULL");
		return false;
	}

	png_struct *pPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pPngStruct)
	{
		NPNG_ERROR("failed to initialize libpng.");
		return false;
	}

	png_info *pPngInfo = png_create_info_struct(pPngStruct);
	if(!pPngInfo)
	{
		NPNG_ERROR("failed to initialize libpng.");
		png_destroy_read_struct(&pPngStruct, NULL, NULL);
		return false;
	}

// start reading the file

	unsigned char *dataPtr = data;
	unsigned char **rowPtrs = NULL;

	// check png signature, make sure file is valid png
	if(png_sig_cmp(dataPtr, 0, PNGSIGSIZE) != 0)
	{
		NPNG_ERROR("couldn't read png header.");
		return false;
	}

	bool ret = false;

	// set pointer for handling errors during loading
	if(setjmp(png_jmpbuf(pPngStruct)) == 0)
	{
		png_set_read_fn(pPngStruct, &dataPtr, &read_mem_data);
		png_set_sig_bytes(pPngStruct, PNGSIGSIZE);
		dataPtr += PNGSIGSIZE;

		png_read_info(pPngStruct, pPngInfo);
		w = png_get_image_width(pPngStruct, pPngInfo);
		h = png_get_image_height(pPngStruct, pPngInfo);

		InitPngSettings(pPngStruct, pPngInfo);
		
		pixels = new unsigned char[w * h * 4];
		rowPtrs = new unsigned char*[h];
		
		for(unsigned int y = 0; y < h; y++)
		{
			rowPtrs[y] = pixels + (y * w * 4);
		}

		png_read_image(pPngStruct, rowPtrs);
		
		ret = true;
	}
	else // cleanup on error
	{
		NPNG_ERROR("Failed to load png file.");

		if(pixels != NULL) delete [] pixels;
		
		pixels = NULL;
		w = 0;
		h = 0;
		ret = false;
	}

	if(pPngStruct)	png_destroy_read_struct(&pPngStruct, &pPngInfo, NULL);
	if(rowPtrs)		delete [] rowPtrs;
	
	return ret;
}

bool NPng::SavePNG(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer)
{
	FILE *file = NULL;
	volatile unsigned int *row = NULL;

	file = fopen(filename, "wb");
	if(!file)
	{
		NPNG_ERROR("failed to open the specified file.");
		return false;
	}

	png_struct *pPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pPngStruct)
	{
		NPNG_ERROR("failed to initialize libpng.");
		fclose(file);
		return false;
	}

	png_info *pPngInfo = png_create_info_struct(pPngStruct);
	if(!pPngInfo)
	{
		NPNG_ERROR("failed to initialize libpng.");
		fclose(file);
		png_destroy_read_struct(&pPngStruct, NULL, NULL);
		return false;
	}

	bool ret = false;

	if(setjmp(png_jmpbuf(pPngStruct)) == 0)
	{
		png_init_io(pPngStruct, file);

		png_set_IHDR(pPngStruct,
					 pPngInfo,
					 width,
					 height,
					 8,
					 PNG_COLOR_TYPE_RGBA,
					 PNG_INTERLACE_NONE,
					 PNG_COMPRESSION_TYPE_BASE,
					 PNG_FILTER_TYPE_BASE);

		png_write_info(pPngStruct, pPngInfo);

		row = new unsigned int[width];
		unsigned int *r = (unsigned int*)buffer;

		for(unsigned int y = 0; y < height; y++)
		{
			for(unsigned int x = 0; x < width; x++)
			{
				memcpy((unsigned int*)&row[x], r + x, 4);
			}

			png_write_row(pPngStruct, (png_const_bytep)row);
			r += width;
		}

		ret = true;
	}
	else
	{
		NPNG_ERROR("Failed to save png file.");
		ret = false;
	}

	fclose(file);
	if(row) delete [] row;
	png_destroy_write_struct(&pPngStruct, &pPngInfo);
	
	return ret;
}

void NPng::Close()
{
	if(pixels)
	{
		delete [] pixels;
		pixels = NULL;
	}

	w = h = 0;
}

void NPng::FlipHorizontal()
{
	if(pixels == NULL)
		return;

	unsigned int nPixels = w * h;
	unsigned int *newPixels = new unsigned int[nPixels];

	for(unsigned int y = 0; y < h; ++y)
	{
		for(unsigned int x = 0; x < w; ++x)
		{
			newPixels[y * w + x] = ((unsigned int*)pixels)[y * w + (w - 1 - x)];
		}
	}

	unsigned char *oldPixels = pixels;
	pixels = (unsigned char*)newPixels;
	delete [] oldPixels;
}

void NPng::FlipVertical()
{
	if(pixels == NULL)
		return;

	unsigned int nPixels = w * h;
	unsigned int *newPixels = new unsigned int[nPixels];

	for(unsigned int y = 0; y < h; ++y)
	{
		for(unsigned int x = 0; x < w; ++x)
		{
			newPixels[y * w + x] = ((unsigned int*)pixels)[(h - 1 - y) * w + x];
		}
	}

	unsigned char *oldPixels = pixels;
	pixels = (unsigned char*)newPixels;
	delete [] oldPixels;
}