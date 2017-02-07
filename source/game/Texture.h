/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"
#include "includes.h"
#include "Path.h"

class Texture : public Object
{
public:
	enum class WrapMode
	{
		Clamp,
		Repeat,
	};

	Texture();
	Texture(const string &filename);
	~Texture();

	bool Open(const string &filename);
	void Close();
	bool IsOpen() const;
	
	int width() const;
	int height() const;
	uint32_t textureID() const;
	const Path filename() const;
	uint32_t channelsCount() const;
	WrapMode wrapMode() const;
	void wrapMode(WrapMode setWrapMode);
protected:
	Path _filename;
	GLuint _textureID;
	uint32_t _width;
	uint32_t _height;
	WrapMode _wrapMode;
	bool _isOpen;
};
