/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_GL_Sprite_H
#define INC_GL_Sprite_H

#include <afxwin.h>
#include "definitions.h"
#include <NPng.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL_Screen.h"
#include "vecmath.h"
#include <assert.h>

class GL_Sprite
{
public:

	GL_Sprite();
	GL_Sprite(const char *filename); // call this one for PC port
	GL_Sprite(int resID);
	~GL_Sprite();

	// load a PNG file
	int OpenImage(const char *filename);
	int LoadResBitmap(int resID);

	// this sprite will not draw if it is outside viewRect.
	// viewRect can be NULL.
	void Draw(GL_Screen *dest, Rect<float> *viewRect, float shade = 1.0f, float alpha = 1.0f);

	void SetX(int X);
	void SetY(int Y);
	void SetX(float X);
	void SetY(float Y);
	void SetPos(int X, int Y);
	void SetPos(float X, float Y);
	void SetNumRows(int rows);
	void SetNumCols(int columns);
	void SetRow(int row);
	void SetColumn(int column);
	void SetFrame(int row, int column);
	void Rotate(float degrees);
	void SetAngle(float degrees);
	void SetScale(float multiplier);
	void SetQuad(const Quad< Vec2<float> > &quad);

	
	float GetX();
	float GetY();
	int GetWidth();
	int GetHeight();
	int GetSheetWidth();
	int GetSheetHeight();
	int GetNumRows();
	int GetNumCols();
	int GetRow();
	int GetColumn();
	float GetAngle();
	const Rect<float> *GetRect();
	const Quad< Vec2<float> > &GetQuad();

	// called automatically when needed, do not call this
	inline void UpdateTransform();

	// transform will be updated on draw if this is set to false
	bool transformValid;


private:
	void CalcRect();
	void Clear();

	GLuint textureId;
	
	unsigned int w, h;

	float rowHeight;
	float colWidth;

	float nRows;
	float nColumns;

	float row;
	float column;

	float angle;
	float scale;

	Vec2<float> pos;
	Mat4<float> mtxScale;
	Mat4<float> mtxRotation;
	
	Mat4<float> transform;
	Rect<float> rect;
	Rect<float> tRect;

	Quad< Vec2<float> > img; // image corners
	Quad< Vec2<float> > tex; // texture coordinates
	Quad< Vec2<float> > tmp; // for caluculating bounding rect
};

#endif /* INC_GL_Sprite_H */