/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <stdio.h>

class NPng
{
public:
	NPng();
	NPng(const char *filename);
	NPng(FILE *file, long long length);
	NPng(unsigned char *data, long long length);
	~NPng();

	bool LoadPNG(const char *filename);
	bool LoadFromFile(FILE *file, long long length);
	bool LoadFromMemory(unsigned char *data, long long length);
	static bool SavePNG(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer);
	void Close();

	unsigned int GetWidth();
	unsigned int GetHeight();
	unsigned char *GetPixels();

	void FlipHorizontal();
	void FlipVertical();

private:

	unsigned char *pixels;
	unsigned int w;
	unsigned int h;
};
