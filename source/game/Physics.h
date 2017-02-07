/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "PQB2Shapes.h"
#include "Trace.h"
#include <memory>
#include <cassert>

using namespace std;

class Object;
class CollisionProxy;
class b2World;
class RigidBody;
struct b2Vec2;

struct Contact
{
	RigidBody* body;
	vec2f point;
	vec2f normal;

	Contact(RigidBody* body, vec2f point, vec2f normal);
};

class Physics
{
	shared_ptr<b2World> _world;
	shared_ptr<CollisionProxy> _collisionProxy;
public:
	Physics();
	~Physics();

	void Update();

	b2World *world();

	static float meterScaleFactor;
	static float pixelScaleFactor;

	static vec2f toMeters(const vec2f &v);
	static vec2f toMeters(float x, float y);
	static float toMeters(float f);
	static vec2f toPixels(const vec2f &v);
	static vec2f toPixels(const b2Vec2 &v);
	static float toPixels(float f);
};
