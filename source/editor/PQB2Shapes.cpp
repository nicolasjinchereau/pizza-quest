/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQB2Shapes.h"

///////////////////////////////////////////////////
//    SHAPE
///////////////////////////////////////////////////

PQB2Shape::PQB2Shape()
{
	type = SHAPE_BASE;
	selected = false;
}

PQB2Shape::~PQB2Shape()
{
	
}

///////////////////////////////////////////////////
//    CIRCLE
///////////////////////////////////////////////////

PQB2Circle::PQB2Circle(Vec2<float> center, float radius)
	: center(center), radius(radius)
{
	type = SHAPE_CIRCLE;
}

PQB2Circle::~PQB2Circle()
{

}

void PQB2Circle::Move(float x, float y)
{
	center.x += x;
	center.y += y;
}

int PQB2Circle::HitTest(int x, int y)
{
	Vec2<float> p((float)x, (float)y);

	float d = (p - center).Length() - radius;

	if(d >= SHAPE_SELECTION || d <= -SHAPE_SELECTION)
		return -1;

	return 0;
}

void PQB2Circle::Draw(GL_Screen *dest, float x, float y, float scale)
{
	Vec2<float> p(center.x * scale + x, center.y * scale + y);
	GL_DrawCircle(dest, p, radius * scale, SHAPE_COLOR);
	GL_DrawCircle(dest, p, 2 * scale, VERT_COLOR);
}

///////////////////////////////////////////////////
//    POLYGON
///////////////////////////////////////////////////

PQB2Polygon::PQB2Polygon(Vec2<float> *vertices, int nVerts)
{
	assert(nVerts <= 8);

	type = SHAPE_POLYGON;
	this->nVerts = nVerts;

	memset(this->vertices, 0, sizeof(Vec2<float>) * 8);

	for(int i = 0; i < nVerts; i++)
		this->vertices[i] = vertices[i];
}

PQB2Polygon::PQB2Polygon(const PQB2Polygon &polygon)
{
	type = SHAPE_POLYGON;
	this->nVerts = polygon.nVerts;
	memset(this->vertices, 0, sizeof(Vec2<float>) * 8);

	for(int i = 0; i < nVerts; i++)
		vertices[i] = polygon.vertices[i];
}

PQB2Polygon::~PQB2Polygon()
{
	
}

void PQB2Polygon::Move(float x, float y)
{
	for(int i = 0; i < nVerts; i++)
	{
		vertices[i].x += x;
		vertices[i].y += y;
	}
}

int PQB2Polygon::HitTest(int x, int y)
{
	Vec2<float> p((float)x, (float)y);

	int start = nVerts - 1;
	int target = 0;

	float selDist = SHAPE_SELECTION;
	int selIndex = -1;

	Vec2<float> vec, cp, pv;

	while(target != nVerts)
	{
		vec = vertices[target] - vertices[start];
		pv = p - vertices[start];

		float t = (pv * vec) / (vec * vec);
		float len = vec.Length();
		float m = t * len;

		if((m >= -SHAPE_SELECTION) && (m <= len + SHAPE_SELECTION))
		{
			cp = vec * t;
			float dist = cp.Distance(pv);
			if(dist <= selDist)
			{
				selDist = dist;

				if(t >= 0.5f)
					selIndex = target;
				else
					selIndex = start;
			}
		}

		start = target++;
	}

	return selIndex;
}

void PQB2Polygon::Draw(GL_Screen *dest, float x, float y, float scale)
{
	int start = nVerts - 1;
	int target = 0;

	while(target != nVerts)
	{
		Vec2<float> p1(vertices[start].x * scale + x, vertices[start].y * scale + y);
		Vec2<float> p2(vertices[target].x * scale + x, vertices[target].y * scale + y);
		GL_DrawLine(dest, p1, p2, SHAPE_COLOR);
		GL_DrawCircle(dest, p1, 2 * scale, VERT_COLOR);
		start = target++;
	}
}

///////////////////////////////////////////////////
//    BOX
///////////////////////////////////////////////////

PQB2Box::PQB2Box(Vec2<float> center, float halfWidth, float halfHeight, float angle)
	: center(center), halfWidth(halfWidth), halfHeight(halfHeight), angle(angle)
{
	type = SHAPE_BOX;
}

PQB2Box::~PQB2Box()
{

}

void PQB2Box::Move(float x, float y)
{
	center.x += x;
	center.y += y;
}

int PQB2Box::HitTest(int x, int y)
{
	Vec2<float> p((float)x, (float)y);

	Vec2<float> vertices[4];
	vertices[0].set(-halfWidth, -halfHeight);
	vertices[1].set( halfWidth, -halfHeight);
	vertices[2].set( halfWidth,  halfHeight);
	vertices[3].set(-halfWidth,  halfHeight);

	Mat3<float> rot;
	Mat3<float> trns;

	rot.SetRotation(DEGREES(angle));
	trns.SetTranslation(center.x, center.y);

	Mat3<float> mtx = trns * rot;
	
	vertices[0] = trns * rot * vertices[0];
	vertices[1] = trns * rot * vertices[1];
	vertices[2] = trns * rot * vertices[2];
	vertices[3] = trns * rot * vertices[3];

	int start = 3;
	int target = 0;

	Vec2<float> vec, cp, pv;

	while(target != 4)
	{
		vec = vertices[target] - vertices[start];
		pv = p - vertices[start];

		float t = (pv * vec) / (vec * vec);
		float len = vec.Length();
		float m = t * len;

		if((m >= -SHAPE_SELECTION) && (m <= len + SHAPE_SELECTION))
		{
			cp = vec * t;
			float dist = cp.Distance(pv);
			if(dist <= SHAPE_SELECTION)
			{
				return 0;
			}
		}

		start = target++;
	}

	return -1;
}

void PQB2Box::Draw(GL_Screen *dest, float x, float y, float scale)
{
	Vec2<float> p1(-halfWidth, -halfHeight);
	Vec2<float> p2( halfWidth, -halfHeight);
	Vec2<float> p3( halfWidth,  halfHeight);
	Vec2<float> p4(-halfWidth,  halfHeight);

	Mat3<float> rot;
	Mat3<float> scl;
	Mat3<float> trns;

	rot.SetRotation(DEGREES(angle));
	trns.SetTranslation(center.x * scale + x, center.y * scale + y);
	scl.SetScale(scale, scale);

	Mat3<float> mtx = trns * scl * rot;
	
	p1 = mtx * p1;
	p2 = mtx * p2;
	p3 = mtx * p3;
	p4 = mtx * p4;

	GL_DrawLine(dest, p1, p2, SHAPE_COLOR);
	GL_DrawLine(dest, p2, p3, SHAPE_COLOR);
	GL_DrawLine(dest, p3, p4, SHAPE_COLOR);
	GL_DrawLine(dest, p4, p1, SHAPE_COLOR);
	GL_DrawCircle(dest, p1, 2 * scale, VERT_COLOR);
	GL_DrawCircle(dest, p2, 2 * scale, VERT_COLOR);
	GL_DrawCircle(dest, p3, 2 * scale, VERT_COLOR);
	GL_DrawCircle(dest, p4, 2 * scale, VERT_COLOR);
}

///////////////////////////////////////////////////
//    EDGE
///////////////////////////////////////////////////

PQB2Edge::PQB2Edge(Vec2<float> p1, Vec2<float> p2)
	: p1(p1), p2(p2)
{
	type = SHAPE_EDGE;
}

PQB2Edge::~PQB2Edge()
{

}

void PQB2Edge::Move(float x, float y)
{
	p1.x += x;
	p1.y += y;
	p2.x += x;
	p2.y += y;
}

int PQB2Edge::HitTest(int x, int y)
{
	Vec2<float> p((float)x, (float)y);

	Vec2<float> vec = p2 - p1;
	Vec2<float> pv = p - p1;

	float t = (pv * vec) / (vec * vec);
	
	float len = vec.Length();
	float m = t * len;

	if((m >= -SHAPE_SELECTION) && (m <= len + SHAPE_SELECTION))
	{
		Vec2<float> cp = vec * t;
		float dist = cp.Distance(pv);
		
		if(dist <= SHAPE_SELECTION)
		{
			if(t >= 0.5f)
				return 1; // p2
			else
				return 0; // p1
		}
	}

	return -1;
}

void PQB2Edge::Draw(GL_Screen *dest, float x, float y, float scale)
{
	Mat3<float> scl;
	Mat3<float> trns;

	scl.SetScale(scale, scale);
	trns.SetTranslation(x, y);
	
	Mat3<float> mtx = trns * scl;
	
	Vec2<float> p1t;
	Vec2<float> p2t;
	p1t.LeftTransform(mtx, p1);
	p2t.LeftTransform(mtx, p2);

	GL_DrawLine(dest, p1t, p2t, SHAPE_COLOR);
	GL_DrawCircle(dest, p1t, 2 * scale, VERT_COLOR);
	GL_DrawCircle(dest, p2t, 2 * scale, VERT_COLOR);
}