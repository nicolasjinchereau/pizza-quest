/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Physics.h"
#include "RigidBody.h"
#include "Object.h"
#include <Box2D.h>
#include "Time.h"

Contact::Contact(RigidBody* body, vec2f point, vec2f normal)
		: body(body),
		  point(point),
		  normal(normal)
{
}

class CollisionProxy : public b2ContactListener
{
public:
	virtual void CollisionProxy::BeginContact(b2Contact* contact) override
	{
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		
		vec2f contactPoint = Physics::toPixels(vec2f(manifold.points[0].x, manifold.points[0].y));
		vec2f normalA = Physics::toPixels(vec2f(manifold.normal.x, manifold.normal.y));
		vec2f normalB = -normalA;
		
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		RigidBody* rbA = (RigidBody*)bodyA->GetUserData();
		RigidBody* rbB = (RigidBody*)bodyB->GetUserData();

		auto parentA = rbA->parent();
		auto parentB = rbB->parent();

		if(parentA) parentA->OnCollisionEnter(Contact(rbB, contactPoint, normalA));
		if(parentB) parentB->OnCollisionEnter(Contact(rbA, contactPoint, normalB));
	}

	virtual void CollisionProxy::EndContact(b2Contact* contact) override
	{
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		
		vec2f contactPoint = Physics::toPixels(vec2f(manifold.points[0].x, manifold.points[0].y));
		vec2f normalA = Physics::toPixels(vec2f(manifold.normal.x, manifold.normal.y));
		vec2f normalB = -normalA;

		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		RigidBody* rbA = (RigidBody*)bodyA->GetUserData();
		RigidBody* rbB = (RigidBody*)bodyB->GetUserData();

		auto parentA = rbA->parent();
		auto parentB = rbB->parent();

		if(parentA) parentA->OnCollisionExit(Contact(rbB, contactPoint, normalA));
		if(parentB) parentB->OnCollisionExit(Contact(rbA, contactPoint, normalB));
	}
};

Physics::Physics()
	: _world(make_shared<b2World>(b2Vec2(0, 0))),
	  _collisionProxy(make_shared<CollisionProxy>())
{
	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(false);
	_world->SetContactListener(_collisionProxy.get());
}

Physics::~Physics()
{

}

void Physics::Update()
{
	_world->Step(Time::deltaTime(), 10, 10);
}

b2World *Physics::world()
{
	return _world.get();
}

float Physics::meterScaleFactor = 10.0f / 128.0f;
float Physics::pixelScaleFactor = 128.0f / 10.0f;

vec2f Physics::toMeters(const vec2f &v)
{
	return v * meterScaleFactor;
}

vec2f Physics::toMeters(float x, float y)
{
	return vec2f(x * meterScaleFactor,
				 y * meterScaleFactor);
}

float Physics::toMeters(float f)
{
	return f * meterScaleFactor;
}

vec2f Physics::toPixels(const vec2f &v)
{
	return v * pixelScaleFactor;
}

vec2f Physics::toPixels(const b2Vec2 &v)
{
	return vec2f(v.x * pixelScaleFactor, v.y * pixelScaleFactor);
}

float Physics::toPixels(float f)
{
	return f * pixelScaleFactor;
}
