/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "vecmath.h"
#include "GL_Screen.h"
#include "GL_Draw.h"
#include "History.h"

#define RULER_HIT_DIST 10

enum RULERTYPE
{
	RULER_BASE,
	RULER_HORIZONTAL,
	RULER_VERTICAL,
};

class Ruler : public Undoable
{
public:
	Uint8 type;
	bool selected;
	float x;
	float y;

	Ruler() : type(RULER_BASE), selected(false), x(0), y(0){}
	
	Ruler(const Ruler &that) : selected(that.selected)
	{
		this->type = that.type;
		this->selected = that.selected;
		this->x = that.x;
		this->y = that.y;
	}

	virtual void Swap(Undoable *object)
	{
		Ruler &obj = *(Ruler*)object;

		SWAP<Uint8>(this->type, obj.type);
		SWAP<bool>(this->selected, obj.selected);
		SWAP<float>(this->x, obj.x);
		SWAP<float>(this->y, obj.y);
	}

	virtual void MoveTo(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	
	virtual Undoable *GetCopy() = 0;
	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, bool active = false, float offset_x = 0, float offset_y = 0) = 0;
	virtual bool HitTest(float wx, float wy) = 0;
};

class HRuler : public Ruler
{
public:

	HRuler()
	{
		type = RULER_HORIZONTAL;
	}

	HRuler(const HRuler &that) : Ruler(that){}

	virtual Undoable *GetCopy()
	{
		return new HRuler(*this);
	}

	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, bool active = false, float offset_x = 0, float offset_y = 0)
	{
		float off_y = y + offset_y;

		if(off_y >= viewRect.y && off_y < viewRect.y + viewRect.h)
		{
			Color color;

			if(selected)
				color = Color(255, 255, 255);
			else
				color = active? Color(255, 0, 0) : Color(200, 0, 0);

			const float thickness = active? 5.0f : 3.0f;
			GL_DrawLine(dest, Vec2<float>(viewRect.x, off_y), Vec2<float>(viewRect.x + viewRect.w, off_y), color, thickness);	
		}
	}

	virtual bool HitTest(float wx, float wy)
	{
		return wy <= y + RULER_HIT_DIST && wy >= y - RULER_HIT_DIST;
	}
};

class VRuler : public Ruler
{
public:

	VRuler()
	{
		type = RULER_VERTICAL;
	}

	VRuler(const VRuler &that) : Ruler(that){}

	virtual Undoable *GetCopy()
	{
		return new VRuler(*this);
	}

	virtual void Draw(GL_Screen *dest, const Rect<float> &viewRect, bool active = false, float offset_x = 0, float offset_y = 0)
	{
		float off_x = x + offset_x;

		if(off_x >= viewRect.x && off_x < viewRect.x + viewRect.w)
		{
			Color color;

			if(selected)
				color = Color(255, 255, 255);
			else
				color = active? Color(255, 0, 0) : Color(200, 0, 0);

			const float thickness = active? 5.0f : 3.0f;
			GL_DrawLine(dest, Vec2<float>(off_x, viewRect.y), Vec2<float>(off_x, (float)viewRect.y + viewRect.h), color, thickness);
		}
	}

	virtual bool HitTest(float wx, float wy)
	{
		return wx <= x + RULER_HIT_DIST && wx >= x - RULER_HIT_DIST;
	}
};