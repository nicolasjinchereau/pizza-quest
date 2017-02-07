/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "DrawBuffer.h"

DrawBuffer::DrawBuffer()
{
	_hBuffer = -1;
	_size = 0;
	_type = Type::None;
	_dynamic = false;
}

DrawBuffer::~DrawBuffer()
{
	if(glIsBuffer(_hBuffer))
		glDeleteBuffers(1, &_hBuffer);
}

void DrawBuffer::SetData(const void *data, uint32_t size, Type type, bool dynamic)
{
	ClearData();

	_type = type;
	_size = size;
	_dynamic = dynamic;

	if(_type != Type::None)
	{
		GLenum bufferTypes[3] = { 0, GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER };

		GLenum bufferType = bufferTypes[(int)_type];

		glGenBuffers(1, &_hBuffer);
		glBindBuffer(bufferType, _hBuffer);

		glBufferData(bufferType, size, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if(data)
		{
			void *p = glMapBuffer(bufferType, GL_WRITE_ONLY);
			memcpy(p, data, size);
			glUnmapBuffer(bufferType);
		}

		glBindBuffer(bufferType, 0);
	}
}

void DrawBuffer::UpdateData(const void *data, uint32_t offset, uint32_t size)
{
	if(size <= _size)
	{
		GLenum bufferTypes[3] = { 0, GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER };
		GLenum bufferType = bufferTypes[(int)_type];

		glBindBuffer(bufferType, _hBuffer);
		glBufferSubData(bufferType, offset, size, data);
		glBindBuffer(bufferType, 0);
	}
}

void DrawBuffer::ClearData()
{
	if(glIsBuffer(_hBuffer))
		glDeleteBuffers(1, &_hBuffer);

	_hBuffer = -1;
	_type = Type::None;
	_size = 0;
	_dynamic = false;
}

uint32_t DrawBuffer::size() const
{
	return _size;
}

DrawBuffer::Type DrawBuffer::type() const
{
	return _type;
}

bool DrawBuffer::empty() const
{
	return _size == 0;
}

uint32_t DrawBuffer::bufferID() const
{
	return _hBuffer;
}
