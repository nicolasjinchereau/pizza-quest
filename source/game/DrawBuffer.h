/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "includes.h"

class DrawBuffer
{
public:
	enum class Type
	{
		None,
		VertexData,
		IndexData,
	};

	DrawBuffer();
	~DrawBuffer();

	void SetData(const void *data, uint32_t size, Type type, bool dynamic = false);
	void UpdateData(const void *data, uint32_t offset, uint32_t size);
	void ClearData();

	uint32_t size() const;
	Type type() const;
	bool empty() const;
	uint32_t bufferID() const;

private:
	GLuint _hBuffer;
	Type _type;
	uint32_t _size;
	bool _dynamic;
};
