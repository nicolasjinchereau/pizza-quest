/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "vecmath.h"
#include "GL_Screen.h"
#include "GL_Draw.h"
#include "list.h"
#include <float.h>
#include <memory>

using namespace std;

enum PQB2_SHAPE
{
	SHAPE_BASE,
	SHAPE_CIRCLE,
	SHAPE_POLYGON,
	SHAPE_BOX,
	SHAPE_EDGE,
};

#define SHAPE_SELECTION (5.0f)
#define SHAPE_SELECTION_SQ (SHAPE_SELECTION * SHAPE_SELECTION)

#define DEF_CLR			Color(255, 0, 0)
#define SEL_CLR			Color(255, 255, 255)
#define SHAPE_COLOR		(selected ? SEL_CLR : DEF_CLR)
#define VERT_COLOR		Color(0, 0, 255)


class PQB2Shape;
class PQB2Circle;
class PQB2Polygon;
class PQB2Box;
class PQB2Edge;

typedef shared_ptr<PQB2Shape>		PQB2ShapePtr;
typedef shared_ptr<PQB2Circle>		PQB2CirclePtr;
typedef shared_ptr<PQB2Polygon>		PQB2PolygonPtr;
typedef shared_ptr<PQB2Box>			PQB2BoxPtr;
typedef shared_ptr<PQB2Edge>		PQB2EdgePtr;

///////////////////////////////////////////////////
//    SHAPE
///////////////////////////////////////////////////

class PQB2Shape
{
public:
	PQB2Shape();
	virtual ~PQB2Shape();
	
	Uint8 type;
	bool selected;

	virtual void Move(float x, float y) = 0;
	virtual int HitTest(int x, int y) = 0;
	virtual void Draw(GL_Screen *dest, float x, float y, float scale = 1.0f) = 0;
};

///////////////////////////////////////////////////
//    CIRCLE
///////////////////////////////////////////////////

class PQB2Circle : public PQB2Shape
{
public:
	PQB2Circle(Vec2<float> center = Vec2<float>(), float radius = 0.0f);
	virtual ~PQB2Circle();

	Vec2<float> center;
	float radius;

	virtual void Move(float x, float y);
	virtual int HitTest(int x, int y);
	virtual void Draw(GL_Screen *dest, float x, float y, float scale = 1.0f);
};

///////////////////////////////////////////////////
//    POLYGON
///////////////////////////////////////////////////

class PQB2Polygon : public PQB2Shape
{
public:
	PQB2Polygon(Vec2<float> *vertices = NULL, int nVerts = 0);
	PQB2Polygon(const PQB2Polygon &);
	virtual ~PQB2Polygon();

	int nVerts; // max 8 verts. extra vert for creation
	Vec2<float> vertices[9];

	virtual void Move(float x, float y);
	virtual int HitTest(int x, int y);
	virtual void Draw(GL_Screen *dest, float x, float y, float scale = 1.0f);
};

///////////////////////////////////////////////////
//    BOX
///////////////////////////////////////////////////

class PQB2Box : public PQB2Shape
{
public:
	PQB2Box(Vec2<float> center = Vec2<float>(), float halfWidth = 0.0f, float halfHeight = 0.0f, float angle = 0.0f);
	virtual ~PQB2Box();

	Vec2<float> center;
	float halfWidth;
	float halfHeight;
	float angle;

	virtual void Move(float x, float y);
	virtual int HitTest(int x, int y);
	virtual void Draw(GL_Screen *dest, float x, float y, float scale = 1.0f);
};

///////////////////////////////////////////////////
//    EDGE
///////////////////////////////////////////////////

class PQB2Edge : public PQB2Shape
{
public:
	PQB2Edge(Vec2<float> p1 = Vec2<float>(), Vec2<float> p2 = Vec2<float>());
	virtual ~PQB2Edge();
	
	Vec2<float> p1;
	Vec2<float> p2;

	virtual void Move(float x, float y);
	virtual int HitTest(int x, int y);
	virtual void Draw(GL_Screen *dest, float x, float y, float scale = 1.0f);
};