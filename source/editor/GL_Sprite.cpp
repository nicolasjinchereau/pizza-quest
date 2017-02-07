/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "GL_Sprite.h"

GL_Sprite::GL_Sprite()
{
	Clear();
	UpdateTransform();
}

GL_Sprite::GL_Sprite(const char *filename)
{
	Clear();
	int ret = OpenImage(filename);
	assert(ret == 0);
}

GL_Sprite::GL_Sprite(int resID)
{
	Clear();
	int ret = LoadResBitmap(resID);
	assert(ret == 0);
}

void GL_Sprite::Clear()
{
	textureId = 0;
	pos.set(0, 0);
	w = 0;
	h = 0;
	rowHeight = 0.0f;
	colWidth = 0.0f;
	nRows = 1;
	nColumns = 1;
	
	row = 0;
	column = 0;

	angle = 0.0f;
	scale = 1.0f;
}

GL_Sprite::~GL_Sprite()
{
	if(textureId != 0)
	{
		glDeleteTextures(1, &textureId);
	}
}

int GL_Sprite::OpenImage(const char *filename)
{
	NPng image;

	if(!image.LoadPNG(filename))
		return -1;
	
	w = image.GetWidth();
	h = image.GetHeight();

	SetNumRows(1);
	SetNumCols(1);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());

	UpdateTransform();

	return 0;
}

int GL_Sprite::LoadResBitmap(int resID)
{
	int ret = -1;

	HBITMAP hBmp;
	BITMAP	bmp;
	
	hBmp = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(resID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
	if(hBmp)
	{
		GetObject(hBmp, sizeof(bmp), &bmp);
		
		// generate a texture
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// choose the format and alignment for the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//create a bitmap from the resource bitmap's bits
		RGBTRIPLE *image = new RGBTRIPLE[bmp.bmWidth * bmp.bmHeight];
		RGBTRIPLE *pixels = (RGBTRIPLE*)bmp.bmBits;
		int p1, p2;

		for(int y = 0; y < bmp.bmHeight; y++)
		{
			for(int x = 0; x < bmp.bmWidth; x++)
			{
				p1 = (y * bmp.bmWidth) + x;
				p2 = ((bmp.bmHeight - y - 1) * bmp.bmWidth) + x;
				
				image[p1].rgbtRed = pixels[p2].rgbtRed;
				image[p1].rgbtGreen = pixels[p2].rgbtGreen;
				image[p1].rgbtBlue = pixels[p2].rgbtBlue;
			}
		}

		w = bmp.bmWidth;
		h = bmp.bmHeight;

		SetNumRows(1);
		SetNumCols(1);

		// load the bits into memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, (GLvoid*)image);

		delete [] image;
		
		// delete the bitmap data
		DeleteObject(hBmp);

		// success
		ret = 0;
	}

	UpdateTransform();

	return  ret;
}

void GL_Sprite::CalcRect()
{
	rect.x = Min4(tmp.TL.x, tmp.BL.x, tmp.TR.x, tmp.BR.x);
	rect.y = Min4(tmp.TL.y, tmp.BL.y, tmp.TR.y, tmp.BR.y);
	rect.w = (Max4(tmp.TL.x, tmp.BL.x, tmp.TR.x, tmp.BR.x) + 0.5f) - rect.x;
	rect.h = (Max4(tmp.TL.y, tmp.BL.y, tmp.TR.y, tmp.BR.y) + 0.5f) - rect.y;
}

inline void GL_Sprite::UpdateTransform()
{
	if(transformValid)
		return;

	const float radAng = RADIANS(angle);
	const float cf = cosf(radAng) * scale;
	const float sf = sinf(radAng) * scale;

	//transform = mtxTranslation * mtxRotation * mtxScale;

	//transform.Set(cf * scale, -sf * scale, 0.0f, pos.x,
	//				sf * scale, cf * scale, 0.0f, pos.y,
	//				0.0f, 0.0f, 1.0f, 0.0f,
	//				scale, scale, 0.0f, 1);

	transform.Set(cf, -sf, 0.0f, pos.x,
					sf, cf, 0.0f, pos.y,
					0.0f, 0.0f, 1.0f, 0.0f,
					scale, scale, 0.0f, 1);

	tmp.TL.LeftTransform(transform, img.TL);
	tmp.BL.LeftTransform(transform, img.BL);
	tmp.TR.LeftTransform(transform, img.TR);
	tmp.BR.LeftTransform(transform, img.BR);

	rect.x = Min4(tmp.TL.x, tmp.BL.x, tmp.TR.x, tmp.BR.x);
	rect.y = Min4(tmp.TL.y, tmp.BL.y, tmp.TR.y, tmp.BR.y);
	rect.w = (Max4(tmp.TL.x, tmp.BL.x, tmp.TR.x, tmp.BR.x) + 0.5f) - rect.x;
	rect.h = (Max4(tmp.TL.y, tmp.BL.y, tmp.TR.y, tmp.BR.y) + 0.5f) - rect.y;

	transformValid = true;
}

void GL_Sprite::Draw(GL_Screen *dest, Rect<float> *viewRect, float shade, float alpha)
{
	UpdateTransform();

	if(viewRect != NULL	&& viewRect->Intersects(this->rect) == false)
		return; // don't draw this if it's not on screen

	if(dest->MakeCurrent() == 0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glColor4f(shade, shade, shade, alpha);
		glVertexPointer(2, GL_FLOAT, 0, &tmp);
		glTexCoordPointer(2, GL_FLOAT, 0, &tex);
		glDrawArrays(GL_QUADS, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void GL_Sprite::SetX(int X)
{
	pos.x = (float)X;
	transformValid = false;
}

void GL_Sprite::SetY(int Y)
{
	pos.y = (float)Y;
	transformValid = false;
}
void GL_Sprite::SetX(float X)
{
	pos.x = X;
	transformValid = false;
}

void GL_Sprite::SetY(float Y)
{
	pos.y = Y;
	transformValid = false;
}
void GL_Sprite::SetPos(int X, int Y)
{
	pos.x = (float)X;
	pos.y = (float)Y;
	transformValid = false;
}
void GL_Sprite::SetPos(float X, float Y)
{
	pos.x = X;
	pos.y = Y;
	transformValid = false;
}

float GL_Sprite::GetX()
{
	return pos.x;
}

float GL_Sprite::GetY()
{
	return pos.y;
}

int GL_Sprite::GetWidth()
{
	return (int)colWidth;
}

int GL_Sprite::GetHeight()
{
	return (int)rowHeight;
}

int GL_Sprite::GetSheetWidth()
{
	return (int)w;
}

int GL_Sprite::GetSheetHeight()
{
	return (int)h;
}

const Rect<float> *GL_Sprite::GetRect()
{
	return &rect;
}

const Quad< Vec2<float> > &GL_Sprite::GetQuad()
{
	return tmp;
}

void GL_Sprite::Rotate(float degrees)
{
	angle += degrees;
	transformValid = false;
}

void GL_Sprite::SetAngle(float degrees)
{
	angle = degrees;
	transformValid = false;
}

float GL_Sprite::GetAngle()
{
	return angle;
}

void GL_Sprite::SetScale(float multiplier)
{
	scale = multiplier;
	transformValid = false;
}

void GL_Sprite::SetQuad(const Quad< Vec2<float> > &quad)
{
	tmp = quad;
	transformValid = true;
}

void GL_Sprite::SetNumRows(int rows)
{
	if(textureId)
	{
		nRows = (float)rows;

		rowHeight = (float)h / nRows;

		float halfHgt = rowHeight / 2.0f;

		img.TL.y = -halfHgt;
		img.BL.y = halfHgt;
		img.BR.y = halfHgt;
		img.TR.y = -halfHgt;

		SetRow(0);
	}

	transformValid = false;
}

void GL_Sprite::SetNumCols(int columns)
{
	if(textureId)
	{
		nColumns = (float)columns;

		colWidth = (float)w / nColumns;

		float halfWid = colWidth / 2.0f;

		img.TL.x = -halfWid;
		img.BL.x = -halfWid;
		img.BR.x = halfWid;
		img.TR.x = halfWid;

		SetColumn(0);
	}

	transformValid = false;
}

int GL_Sprite::GetNumRows()
{
	return (int)nRows;
}

int GL_Sprite::GetNumCols()
{
	return (int)nColumns;
}

void GL_Sprite::SetRow(int r)
{
	row = (float)r;

	tex.TL.y = row / nRows;
	tex.BL.y = (row + 1.0f) / nRows;
	tex.BR.y = tex.BL.y;
	tex.TR.y = tex.TL.y;
}

void GL_Sprite::SetColumn(int c)
{
	column = (float)c;

	tex.TL.x = column / nColumns;
	tex.BL.x = tex.TL.x;
	tex.BR.x = (column + 1) / nColumns;
	tex.TR.x = tex.BR.x;
}

void GL_Sprite::SetFrame(int r, int c)
{
	if((int)row != r)
	{
		row = (float)r;
		tex.TL.y = row / nRows;
		tex.BL.y = (row + 1.0f) / nRows;
		tex.BR.y = tex.BL.y;
		tex.TR.y = tex.TL.y;
	}

	if((int)column != c)
	{
		column = (float)c;

		tex.TL.x = column / nColumns;
		tex.BL.x = tex.TL.x;
		tex.BR.x = (column + 1) / nColumns;
		tex.TR.x = tex.BR.x;
	}
}

int GL_Sprite::GetRow()
{
	return (int)row;
}

int GL_Sprite::GetColumn()
{
	return (int)column;
}