/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "GL_Draw.h"

void GL_DrawRect(GL_Screen *dest, Rect<int> *rect, Color clr, float thickness)
{
	Rect<float> rc;

	rc.Set((float)rect->x + 0.5f,
		   (float)rect->y + 0.5f,
		   (float)rect->w,
		   (float)rect->h);

	glLineWidth(thickness);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_LINE_LOOP);
		
		glVertex2f(rc.x,		rc.y);
		glVertex2f(rc.x,		rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y);

	glEnd();
}

void GL_DrawRect(GL_Screen *dest, Rect<float> *rect, Color clr, float thickness)
{
	Rect<float> rc;

	rc.Set(rect->x + 0.5f,
		   rect->y + 0.5f,
		   rect->w,
		   rect->h);

	glLineWidth(thickness);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_LINE_LOOP);
		
		glVertex2f(rc.x,		rc.y);
		glVertex2f(rc.x,		rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y);

	glEnd();
}

void GL_FillRect(GL_Screen *dest, Rect<int> *rect, Color clr)
{
	Rect<float> rc;

	rc.Set((float)rect->x + 0.5f,
		   (float)rect->y + 0.5f,
		   (float)rect->w,
		   (float)rect->h);

	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_QUADS);
	
		glNormal3f(0.0, 0.0f, 1.0f);

		glVertex2f(rc.x, rc.y);
		glVertex2f(rc.x, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y);

	glEnd();
}

void GL_FillRect(GL_Screen *dest, Rect<float> *rect, Color clr)
{
	Rect<float> rc;

	rc.Set(rect->x + 0.5f,
		   rect->y + 0.5f,
		   rect->w,
		   rect->h);

	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_QUADS);
	
		glNormal3f(0.0, 0.0f, 1.0f);

		glVertex2f(rc.x, rc.y);
		glVertex2f(rc.x, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y);

	glEnd();
}

void GL_DrawLine(GL_Screen *dest, Vec2<int> &p1, Vec2<int> &p2, Color clr, float thickness)
{
	Vec2<float> P1((float)p1.x + 0.5f, (float)p1.y + 0.5f);
	Vec2<float> P2((float)p2.x + 0.5f, (float)p2.y + 0.5f);

	glLineWidth(thickness);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_LINES);
		glVertex2f(P1.x, P1.y);
		glVertex2f(P2.x, P2.y);
	glEnd();
}

void GL_DrawLine(GL_Screen *dest, Vec2<float> &p1, Vec2<float> &p2, Color clr, float thickness)
{
	Vec2<float> P1(p1.x + 0.5f, p1.y + 0.5f);
	Vec2<float> P2(p2.x + 0.5f, p2.y + 0.5f);

	glLineWidth(thickness);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_LINES);
		glVertex2f(P1.x, P1.y);
		glVertex2f(P2.x, P2.y);
	glEnd();
}

void GL_DrawCircle(GL_Screen *dest, Vec2<float> &center, float radius, Color clr, float thickness)
{
	glLineWidth(thickness);
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_LINE_LOOP);	
		for(float angle = 0.0f; angle < 360; angle += 5)
		{
			glVertex2f(center.x + sinf(RADIANS(angle)) * radius,
					   center.y + cosf(RADIANS(angle)) * radius);
		}
	glEnd();
}

void GL_FillCircle(GL_Screen *dest, Vec2<float> &center, float radius, const Color &clr)
{
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte*)&clr);

	glBegin(GL_TRIANGLE_FAN);
		
		glVertex2f(center.x, center.y);
		for(float angle = 0.0f; angle <= 360.0f; angle += 5)
			glVertex2f(center.x + sinf(angle) * radius,
					   center.y + cosf(angle) * radius);
	glEnd();
}