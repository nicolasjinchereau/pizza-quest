/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Path.h"
#include "NPng.h"
#include "Math.h"
#include "Shader.h"
#include "Trace.h"
#include "bytestream.h"
#include <list>
#include "Camera.h"
#include "Object.h"
#include "Texture.h"
#include "RenderQueue.h"
#include "DrawBuffer.h"

using namespace std;

class Sprite : public Object
{
	friend class Engine;
public:
	Sprite();
	Sprite(const Sprite &other);
	Sprite(const shared_ptr<Texture> &texture);
	Sprite(const shared_ptr<Shader> &shader);
	Sprite(const char *filename);
	Sprite(const char *filename, const shared_ptr<Shader> &shader);
	~Sprite();
	
	// 32 Bit PNG ONLY
	bool Open(const char *filename);
	void Close();

	void SetX(float X);
	void SetY(float Y);
	void SetPos(int X, int Y);
	void SetPos(float X, float Y);
	void SetPos(const vec2f &pos);
	void SetNumRows(int rows);
	void SetNumCols(int columns);
	void SetRow(int row);
	void SetColumn(int column);
	void SetFrame(int row, int column);
	void Rotate(float degrees);
	void SetAngle(float degrees);
	void SetScale(float multiplier);
	void SetVisible(bool visible);
	void SetStatic(bool is_static);
	void SetShader(shared_ptr<Shader> shader);
	void SetTexture(const shared_ptr<Texture> &texture);

	float GetX();
	float GetY();
	float GetWidth();
	float GetHeight();
	int GetSheetWidth();
	int GetSheetHeight();
	int GetNumRows();
	int GetNumCols();
	int GetRow();
	int GetColumn();
	float GetScale();
	shared_ptr<Shader> GetShader();
	shared_ptr<Texture> GetTexture();

	float GetAngle();
	bool IsVisible();
	bool IsStatic();
	bool Intersects(Sprite *sprite);
	bool PointInside(float x, float y);
	const Path &filename() const;

	Rect rect();
	Rect clipBorder();
	void clipBorder(const Rect &setClipBorder);

	virtual void Draw() override;

private:
	
	void _init();

	shared_ptr<Texture> texture;
	shared_ptr<Shader> shader;

	bool visible;
	bool _is_static;
	
	Rect _staticRect;
	DrawBuffer vertexBuffer;
	DrawBuffer texcoordBuffer;
	Rect _clipBorder;
	vec2f pos;
	float angle;
	float scale;
	int nRows;
	int nColumns;
	int row;
	int column;
	float cosAng;
	float sinAng;
	int aPositionID;
	int aTexCoordID;
	int uMainTexID;
	int uMtxMvpID;
	Path _filename;
};
