/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Texture.h"
#include <NPng.h>
#include "bytestream.h"
#include "utils.h"

Texture::Texture()
{
	_textureID = -1;
	_width = 0;
	_height = 0;
	_isOpen = false;
	_wrapMode = WrapMode::Clamp;
}

Texture::Texture(const string &filename)
{
	_textureID = -1;
	_width = 0;
	_height = 0;
	_isOpen = false;
	_wrapMode = WrapMode::Clamp;

	Open(filename);
}

Texture::~Texture()
{
	Close();
}

bool Texture::Open(const string &filename)
{
	Close();

	NPng image;

	bytestream buffer = bytestream_from_file(filename);

	if(buffer.empty()
	|| !image.LoadFromMemory((unsigned char*)buffer.data(), buffer.size()))
	{
		Trace("Failed to open image: ", filename);
		return false;
	}

	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	bool doMipMaps = IsPowerOfTwo(_width) && IsPowerOfTwo(_height);
	int minFilter = doMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
	
	GLint _gl_WrapModes[]= {
		GL_CLAMP_TO_EDGE,
		GL_REPEAT,
	};

	GLint wm = _gl_WrapModes[(int)_wrapMode];

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wm);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wm);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());

	if(doMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	_width = image.GetWidth();
	_height = image.GetHeight();
	_isOpen = true;

	return true;
}

void Texture::Close()
{
	if(glIsTexture(_textureID))
		glDeleteTextures(1, &_textureID);
	
	_textureID = -1;
	_width = 0;
	_height = 0;
	_isOpen = false;
	_filename = "";
}

bool Texture::IsOpen() const
{
	return _isOpen;
}

int Texture::width() const
{
	return _width;
}

int Texture::height() const
{
	return _height;
}

uint32_t Texture::textureID() const
{
	return _textureID;
}

const Path Texture::filename() const
{
	return _filename;
}

uint32_t Texture::channelsCount() const
{
	return 4;
}

Texture::WrapMode Texture::wrapMode() const
{
	return _wrapMode;
}

void Texture::wrapMode(WrapMode setWrapMode)
{
	_wrapMode = setWrapMode;

	if(glIsTexture(_textureID))
	{
		glBindTexture(GL_TEXTURE_2D, _textureID);

		GLint _gl_WrapModes[]= {
			GL_CLAMP_TO_EDGE,
			GL_REPEAT,
		};

		GLint wm = _gl_WrapModes[(int)_wrapMode];

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wm);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wm);
	}
}
