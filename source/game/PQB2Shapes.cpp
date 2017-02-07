/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQB2Shapes.h"

///////////////////////////////////////////////////
//    SHAPE
///////////////////////////////////////////////////

PQB2Shape::PQB2Shape(uint8_t type) : type(type)
{
	selected = false;
}

PQB2Shape::~PQB2Shape()
{
	
}

///////////////////////////////////////////////////
//    CIRCLE
///////////////////////////////////////////////////

PQB2Circle::PQB2Circle(vec2f center, float radius)
	: center(center), radius(radius), PQB2Shape(SHAPE_CIRCLE)
{
	
}

PQB2Circle::~PQB2Circle()
{

}

///////////////////////////////////////////////////
//    POLYGON
///////////////////////////////////////////////////

PQB2Polygon::PQB2Polygon(vec2f *vertices, int nVerts)
	: PQB2Shape(SHAPE_POLYGON), nVerts(nVerts)
{
	memset(this->vertices, 0, sizeof(vec2f) * 8);
	memcpy(this->vertices, vertices, sizeof(vec2f) * nVerts);
}

PQB2Polygon::~PQB2Polygon()
{
	
}

///////////////////////////////////////////////////
//    BOX
///////////////////////////////////////////////////

PQB2Box::PQB2Box(vec2f center, float halfWidth, float halfHeight, float angle)
	: PQB2Shape(SHAPE_BOX), center(center), halfWidth(halfWidth), halfHeight(halfHeight), angle(angle)
{
	
}

PQB2Box::~PQB2Box()
{

}

///////////////////////////////////////////////////
//    EDGE
///////////////////////////////////////////////////

PQB2Edge::PQB2Edge(vec2f p1, vec2f p2)
	: PQB2Shape(SHAPE_EDGE), p1(p1), p2(p2)
{
	
}

PQB2Edge::~PQB2Edge()
{

}
