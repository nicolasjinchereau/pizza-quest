/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Sprite.h"
#include "Engine.h"
#include "Graphics.h"
#include <fstream>
#include <memory>
using namespace std;

Sprite::Sprite()
{
	_init();
	SetShader(Graphics::defaultShader());
}

Sprite::Sprite(const Sprite &other)
{
	category = other.category;
	layer = other.layer;
	texture = other.texture;
	visible = other.visible;
	_is_static = other._is_static;
	shader = other.shader;
	_staticRect = other._staticRect;
	_clipBorder = other._clipBorder;
	pos = other.pos;
	angle = other.angle;
	scale = other.scale;
	nRows = other.nRows;
	nColumns = other.nColumns;
	row = other.row;
	column = other.column;
	cosAng = other.cosAng;
	sinAng = other.sinAng;
	_filename = other._filename;

	vertexBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
	texcoordBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
}

Sprite::Sprite(const shared_ptr<Texture> &texture)
{
	_init();
	SetShader(Graphics::defaultShader());
	this->texture = texture;
}

Sprite::Sprite(const shared_ptr<Shader> &shader)
{
	_init();
	SetShader(shader);
}

Sprite::Sprite(const char *filename)
{
	_init();
	Open(filename);
	SetShader(Graphics::defaultShader());
}

Sprite::Sprite(const char *filename, const shared_ptr<Shader> &shader)
{
	_init();
	Open(filename);
	SetShader(shader);
}

void Sprite::_init()
{
	category = 0xFFFFFFFF;

	texture = make_shared<Texture>();
	_clipBorder.Set(0, 0, 0, 0);
	pos.set(0, 0);
	nRows = 1;
	nColumns = 1;
	
	row = 0;
	column = 0;

	angle = 0.0f;
	cosAng = 1.0f;
	sinAng = 0.0f;
	scale = 1.0f;

	visible = true;
	_is_static = false;

	aPositionID = -1;
	aTexCoordID = -1;
	uMainTexID = -1;
	uMtxMvpID = -1;

	vertexBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
	texcoordBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
}

Sprite::~Sprite()
{
	
}

bool Sprite::Open(const char *filename)
{
	_filename = filename;
	visible = texture->Open(_filename);

	if(visible)
	{
		SetNumRows(1);
		SetNumCols(1);
	}

	return visible;
}

void Sprite::Close()
{
	_init();
}

void Sprite::Draw()
{
	if(!Camera::activeCamera() || !visible || !shader || vertexBuffer.empty())
		return;

	shader->SetActive();
	shader->SetUniform(uMainTexID, texture.get());
	shader->SetUniform(uMtxMvpID, Camera::activeCamera()->matrix());

	if(_is_static)
	{
		if(!Camera::activeCamera()->viewRect.Intersects(_staticRect))
			return;
	}
	else
	{
		mat3f xf(cosAng * scale, -sinAng * scale, pos.x,
				 sinAng * scale, cosAng * scale, pos.y,
				 scale, scale, 1);

		float rowHeight = (float)texture->height() / nRows;
		float colWidth = (float)texture->width() / nColumns;

		float hw = colWidth * 0.5f;
		float hh = rowHeight * 0.5f;

		vec2f verts[4] =
		{
			vec2f(xf, -hw + _clipBorder.x, -hh + _clipBorder.y),
			vec2f(xf, -hw + _clipBorder.x,  hh - _clipBorder.h),
			vec2f(xf,  hw - _clipBorder.w, -hh + _clipBorder.y),
			vec2f(xf,  hw - _clipBorder.w,  hh - _clipBorder.h),
		};

		Rect rc;

		rc.x = Min4(verts[0].x, verts[1].x, verts[2].x, verts[3].x);
		rc.y = Min4(verts[0].y, verts[1].y, verts[2].y, verts[3].y);
		rc.w = Max4(verts[0].x, verts[1].x, verts[2].x, verts[3].x) + 0.5f - rc.x;
		rc.h = Max4(verts[0].y, verts[1].y, verts[2].y, verts[3].y) + 0.5f - rc.y;

		if(!Camera::activeCamera()->viewRect.Intersects(rc))
			return;

		Rect _texClip(_clipBorder.x / (float)texture->width(),
					  _clipBorder.y / (float)texture->height(),
					  _clipBorder.w / (float)texture->width(),
					  _clipBorder.h / (float)texture->height());

		float rcp_nRows = 1.0f / (float)nRows;
		float rcp_nColumns = 1.0f / (float)nColumns;
		
		vec2f texcoords[4];
		
		texcoords[0].x = (float)column * rcp_nColumns + _texClip.x;
		texcoords[0].y = (float)row * rcp_nRows + _texClip.y;
		texcoords[1].x = texcoords[0].x;
		texcoords[1].y = ((float)row + 1.0f) * rcp_nRows - _texClip.h;
		texcoords[2].x = ((float)column + 1) * rcp_nColumns - _texClip.w;
		texcoords[2].y = texcoords[0].y;
		texcoords[3].x = texcoords[2].x;
		texcoords[3].y = texcoords[1].y;

		vertexBuffer.UpdateData(verts, 0, 4 * sizeof(vec2f));
		texcoordBuffer.UpdateData(texcoords, 0, 4 * sizeof(vec2f));
	}

	shader->SetVertexBuffer(aPositionID, &vertexBuffer);
	shader->SetVertexBuffer(aTexCoordID, &texcoordBuffer);
	
	Graphics::DrawArray(0, 4, DrawMode::TriangleStrip);
}

void Sprite::SetX(float X)
{
	pos.x = X;
}

void Sprite::SetY(float Y)
{
	pos.y = Y;
}

void Sprite::SetPos(int X, int Y)
{
	pos.x = (float)X;
	pos.y = (float)Y;
}

void Sprite::SetPos(float X, float Y)
{
	pos.x = X;
	pos.y = Y;
}

void Sprite::SetPos(const vec2f &pos)
{
	this->pos = pos;
}

float Sprite::GetX()
{
	return pos.x;
}

float Sprite::GetY()
{
	return pos.y;
}

float Sprite::GetWidth()
{
	return (float)texture->width() / nColumns;
}

float Sprite::GetHeight()
{
	return (float)texture->height() / nRows;
}

int Sprite::GetSheetWidth()
{
	return texture->width();
}

int Sprite::GetSheetHeight()
{
	return texture->height();
}

Rect Sprite::rect()
{
	Rect ret;

	if(_is_static)
	{
		ret = _staticRect;
	}
	else
	{
		mat3f xf(cosAng * scale, -sinAng * scale, pos.x,
				 sinAng * scale, cosAng * scale, pos.y,
				 scale, scale, 1);

		float rowHeight = (float)texture->height() / nRows;
		float colWidth = (float)texture->width() / nColumns;

		float hw = (float)colWidth * 0.5f;
		float hh = (float)rowHeight * 0.5f;

		vec2f verts[4] =
		{
			vec2f(xf, -hw + _clipBorder.x, -hh + _clipBorder.y),
			vec2f(xf, -hw + _clipBorder.x,  hh - _clipBorder.h),
			vec2f(xf,  hw - _clipBorder.w, -hh + _clipBorder.y),
			vec2f(xf,  hw - _clipBorder.w,  hh - _clipBorder.h),
		};

		ret.x = Min4(verts[0].x, verts[1].x, verts[2].x, verts[3].x);
		ret.y = Min4(verts[0].y, verts[1].y, verts[2].y, verts[3].y);
		ret.w = Max4(verts[0].x, verts[1].x, verts[2].x, verts[3].x) + 0.5f - ret.x;
		ret.h = Max4(verts[0].y, verts[1].y, verts[2].y, verts[3].y) + 0.5f - ret.y;
	}

	return ret;
}

Rect Sprite::clipBorder()
{
	return _clipBorder;
}

void Sprite::clipBorder(const Rect &setClipBorder)
{
	_clipBorder = setClipBorder;
}

const Path &Sprite::filename() const
{
	return _filename;
}

bool Sprite::Intersects(Sprite *sprite)
{
	return rect().Intersects(sprite->rect());
}

bool Sprite::PointInside(float x, float y)
{
	return rect().PointInside(x, y);
}

bool Sprite::IsVisible()
{
	return this->visible;
}

bool Sprite::IsStatic()
{
	return _is_static;
}

void Sprite::Rotate(float degrees)
{
	angle += degrees;
	float radAng = RADIANS(angle);
	cosAng = cos(radAng);
	sinAng = sin(radAng);
}

void Sprite::SetAngle(float degrees)
{
	angle = degrees;
	float radAng = RADIANS(angle);
	cosAng = cos(radAng);
	sinAng = sin(radAng);
}

float Sprite::GetAngle()
{
	return angle;
}

void Sprite::SetScale(float multiplier)
{
	scale = multiplier;
}

float Sprite::GetScale()
{
	return scale;
}

void Sprite::SetVisible(bool visible)
{
	this->visible = visible;
}

void Sprite::SetStatic(bool is_static)
{
	if(is_static)
	{
		float rcp_nRows = 1.0f / (float)nRows;
		float rcp_nColumns = 1.0f / (float)nColumns;

		mat3f xf(cosAng * scale, -sinAng * scale, pos.x,
				 sinAng * scale, cosAng * scale, pos.y,
				 scale, scale, 1);
	
		float rowHeight = (float)texture->height() / nRows;
		float colWidth = (float)texture->width() / nColumns;

		float hw = (float)colWidth * 0.5f;
		float hh = (float)rowHeight * 0.5f;

		vec2f staticVerts[4];
		vec2f staticTexCoords[4];

		staticVerts[0] = vec2f(xf, -hw, -hh);
		staticVerts[1] = vec2f(xf, -hw,  hh);
		staticVerts[2] = vec2f(xf,  hw, -hh);
		staticVerts[3] = vec2f(xf,  hw,  hh);

		_staticRect.x = Min4(staticVerts[0].x, staticVerts[1].x, staticVerts[2].x, staticVerts[3].x);
		_staticRect.y = Min4(staticVerts[0].y, staticVerts[1].y, staticVerts[2].y, staticVerts[3].y);
		_staticRect.w = Max4(staticVerts[0].x, staticVerts[1].x, staticVerts[2].x, staticVerts[3].x) + 0.5f - _staticRect.x;
		_staticRect.h = Max4(staticVerts[0].y, staticVerts[1].y, staticVerts[2].y, staticVerts[3].y) + 0.5f - _staticRect.y;

		staticTexCoords[0].x = (float)column * rcp_nColumns;
		staticTexCoords[0].y = (float)row * rcp_nRows;
		staticTexCoords[1].x = staticTexCoords[0].x;
		staticTexCoords[1].y = ((float)row + 1.0f) * rcp_nRows;
		staticTexCoords[2].x = ((float)column + 1) * rcp_nColumns;
		staticTexCoords[2].y = staticTexCoords[0].y;
		staticTexCoords[3].x = staticTexCoords[2].x;
		staticTexCoords[3].y = staticTexCoords[1].y;

		vertexBuffer.SetData(staticVerts, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData);
		texcoordBuffer.SetData(staticTexCoords, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData);
	}
	else
	{
		if(_is_static)
		{
			vertexBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
			texcoordBuffer.SetData(nullptr, 4 * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
		}
	}

	_is_static = is_static;
}

void Sprite::SetNumRows(int rows)
{
	nRows = rows;
}

void Sprite::SetNumCols(int columns)
{
	nColumns = columns;
}

int Sprite::GetNumRows()
{
	return nRows;
}

int Sprite::GetNumCols()
{
	return nColumns;
}

void Sprite::SetRow(int r)
{
	row = r;
}

void Sprite::SetColumn(int c)
{
	column = c;
}

void Sprite::SetFrame(int r, int c)
{
	row = r;
	column = c;
}

int Sprite::GetRow()
{
	return row;
}

int Sprite::GetColumn()
{
	return column;
}

void Sprite::SetShader(shared_ptr<Shader> shader)
{
	this->shader = shader;
	aPositionID = shader->GetAttribID("aPosition");
	aTexCoordID = shader->GetAttribID("aTexCoord");
	uMainTexID  = shader->GetUniformID("uMainTex");
	uMtxMvpID   = shader->GetUniformID("uMtxMVP");

#ifndef NDEBUG
	if(aPositionID == -1) Trace("Shader missing required attribute: vec2 aPosition");
	if(aTexCoordID == -1) Trace("Shader missing required attribute: vec2 aTexCoord");
	if(uMainTexID == -1) Trace("Shader missing required uniform: sampler2D uMainTexID");
	if(uMtxMvpID == -1) Trace("Shader missing required uniform: mat3 uMtxMvpID");
#endif
}

shared_ptr<Shader> Sprite::GetShader()
{
	return shader;
}

void Sprite::SetTexture(const shared_ptr<Texture> &texture)
{
	this->texture = texture;
}

shared_ptr<Texture> Sprite::GetTexture()
{
	return texture;
}
