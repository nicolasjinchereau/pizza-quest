/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <string.h>
#include <float.h>
#include "Math.h"
#include <list>

using namespace std;

// 128 pixels(1 tile) = 10 meters(Box2D)
#define PQB2SCALE(x)		((float)(x) * (10.0f / 128.0f))
#define PQB2SCALE_VEC(v)	((v) * (10.0f / 128.0f))
#define PQPXSCALE(x)		((float)(x) * (128.0f / 10.0f))
#define PQPXSCALE_VEC(v)	((v) * (128.0f / 10.0f))

enum PQB2_SHAPE
{
	SHAPE_BASE,
	SHAPE_CIRCLE,
	SHAPE_POLYGON,
	SHAPE_BOX,
	SHAPE_EDGE,
};

///////////////////////////////////////////////////
//    SHAPE
///////////////////////////////////////////////////

class PQB2Shape
{
public:
	PQB2Shape(uint8_t type = SHAPE_BASE);
	virtual ~PQB2Shape();
	
	uint8_t type;
	bool selected;
};

///////////////////////////////////////////////////
//    CIRCLE
///////////////////////////////////////////////////

class PQB2Circle : public PQB2Shape
{
public:
	PQB2Circle(vec2f center = vec2f(), float radius = 0.0f);
	virtual ~PQB2Circle();

	vec2f center;
	float radius;
};

///////////////////////////////////////////////////
//    POLYGON
///////////////////////////////////////////////////

class PQB2Polygon : public PQB2Shape
{
public:
	PQB2Polygon(vec2f *vertices = NULL, int nVerts = 0);
	virtual ~PQB2Polygon();

	int nVerts; // max 8 verts. extra vert for creation
	vec2f vertices[9];
};

///////////////////////////////////////////////////
//    BOX
///////////////////////////////////////////////////

class PQB2Box : public PQB2Shape
{
public:
	PQB2Box(vec2f center = vec2f(), float halfWidth = 0.0f, float halfHeight = 0.0f, float angle = 0.0f);
	virtual ~PQB2Box();

	vec2f center;
	float halfWidth;
	float halfHeight;
	float angle;
};

///////////////////////////////////////////////////
//    EDGE
///////////////////////////////////////////////////
 
class PQB2Edge : public PQB2Shape
{
public:
	PQB2Edge(vec2f p1 = vec2f(), vec2f p2 = vec2f());
	virtual ~PQB2Edge();
	
	vec2f p1;
	vec2f p2;
};