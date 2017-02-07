/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "RigidBody.h"
#include "Physics.h"
#include <Box2D.h>
#include <cassert>
#include "Engine.h"
#include "State.h"
#include "PQB2Shapes.h"

b2BodyType _rigidBody_Bodytypes[]=
{
	b2_staticBody,
	b2_dynamicBody,
	b2_kinematicBody,
	b2_staticBody,
};

RigidBody::RigidBody()
	: _body(nullptr),
	  _type(Type::Dead),
	  _self(ContactMask::None),
	  _others(ContactMask::None)
{
}

RigidBody::RigidBody(shared_ptr<Physics> physics,
					 const vector<shared_ptr<PQB2Shape>> &shapes,
					 const vec2f &position,
					 float angle,
					 float scale,
					 Type type,
					 ContactMask self,
					 ContactMask others)
	: _physics(physics),
	  _type(Type::Dead),
	  _self(ContactMask::All),
	  _others(ContactMask::All)
{
	assert(physics != nullptr);

	b2BodyDef bodyDef;
	bodyDef.userData = this;
	bodyDef.type = _rigidBody_Bodytypes[(int)type];
	bodyDef.position.Set(Physics::toMeters(position.x), Physics::toMeters(position.y));
	bodyDef.angle = angle;
	
	_body = physics->world()->CreateBody(&bodyDef);

	for(auto shape : shapes)
	{
		switch(shape->type)
		{
		case SHAPE_CIRCLE:
			AddCircle(Physics::toMeters(((PQB2Circle*)shape.get())->center),
				      Physics::toMeters(((PQB2Circle*)shape.get())->radius),
				      scale, 0.1f, 0.2f, 0.0f);
			break;

		case SHAPE_POLYGON:
			AddPolygon(((PQB2Polygon*)shape.get())->vertices,
				       ((PQB2Polygon*)shape.get())->nVerts,
				       scale * Physics::meterScaleFactor, 0.1f, 0.2f, 0.0f);

			break;

		case SHAPE_BOX:
			AddBox(Physics::toMeters(((PQB2Box*)shape.get())->halfWidth),
				   Physics::toMeters(((PQB2Box*)shape.get())->halfHeight),
				   Physics::toMeters(((PQB2Box*)shape.get())->center),
				   Physics::toMeters(((PQB2Box*)shape.get())->angle),
				   scale, 0.1f, 0.2f, 0.0f);

			break;

		case SHAPE_EDGE:
			AddEdge(Physics::toMeters(((PQB2Edge*)shape.get())->p1),
				    Physics::toMeters(((PQB2Edge*)shape.get())->p2),
				    scale, 0.1f, 0.2f, 0.0f);

			break;
		}
	}
}

RigidBody::RigidBody(shared_ptr<Physics> physics,
					 Type type,
					 ContactMask self,
					 ContactMask others)
	: _physics(physics),
	  _type(type),
	  _self(self),
	  _others(others)
{
	assert(physics != nullptr);

	b2BodyDef bodyDef;
	bodyDef.userData = this;
	bodyDef.type = _rigidBody_Bodytypes[(int)type];
	
	_body = physics->world()->CreateBody(&bodyDef);
}

RigidBody::RigidBody(shared_ptr<Physics> physics,
					 const vec2f &position,
					 float angle,
					 Type type,
					 ContactMask self,
					 ContactMask others)
	: _physics(physics),
	  _type(type),
	  _self(self),
	  _others(others)
{
	assert(physics != nullptr);

	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(position.x, position.y);
	bodyDef.angle = angle;
	bodyDef.userData = this;
	bodyDef.type = _rigidBody_Bodytypes[(int)type];
	
	_body = physics->world()->CreateBody(&bodyDef);
}

RigidBody::RigidBody(RigidBody &&other)
{
	this->_physics = move(other._physics);

	this->_body   = other._body;
	this->_self   = other._self;
	this->_others = other._others;
	this->_type   = other._type;

	other._body   = nullptr;
	other._self   = ContactMask::None;
	other._others = ContactMask::None;
	other._type   = Type::Dead;
}

RigidBody& RigidBody::operator=(RigidBody &&other)
{
	this->_physics = move(other._physics);

	this->_body   = other._body;
	this->_self   = other._self;
	this->_others = other._others;
	this->_type   = other._type;

	other._body   = nullptr;
	other._self   = ContactMask::None;
	other._others = ContactMask::None;
	other._type   = Type::Dead;

	return *this;
}

RigidBody::~RigidBody()
{
	if(_body)
	{
		if(auto p = _physics.lock())
		{
			this->_parent.reset();
			p->world()->DestroyBody(_body);
		}
	}
}

Object *RigidBody::owner()
{
	return (Object*)_body->GetUserData();
}

RigidBody::Type RigidBody::type() const
{
	return _type;
}

void RigidBody::type(Type setType)
{
	_body->SetType(_rigidBody_Bodytypes[(int)setType]);
	
	for(auto fix = _body->GetFixtureList();
		fix != nullptr;
		fix = fix->GetNext())
	{
		fix->SetSensor(setType == Type::Trigger);
	}
}

vec2f RigidBody::position() const
{
	auto pos = _body->GetPosition();
	return vec2f(pos.x, pos.y);
}

void RigidBody::position(const vec2f &setPosition)
{
	b2Vec2 pos(setPosition.x, setPosition.y);
	float angle = _body->GetAngle();
	_body->SetTransform(pos, angle);
}

vec2f RigidBody::pixelPosition() const
{
	return Physics::toPixels(_body->GetPosition());
}

void RigidBody::pixelPosition(const vec2f &setPixelPosition)
{
	b2Vec2 pos(setPixelPosition.x * Physics::meterScaleFactor,
			   setPixelPosition.y * Physics::meterScaleFactor);

	float angle = _body->GetAngle();
	_body->SetTransform(pos, angle);
}


float RigidBody::angle() const
{
	return _body->GetAngle();
}

void RigidBody::angle(float setAngle)
{
	auto pos = _body->GetPosition();
	_body->SetTransform(pos, setAngle);
}

float RigidBody::mass() const
{
	return _body->GetMass();
}

void RigidBody::mass(float setMass)
{
	b2CircleShape c;
	c.m_radius = 1.0f;
	c.m_p = b2Vec2_zero;

	b2MassData massData;
	c.ComputeMass(&massData, setMass);
	
	_body->SetMassData(&massData);
}

float RigidBody::inertia() const
{
	return _body->GetInertia();
}

void RigidBody::transform(const vec2f &setPosition, float setAngle)
{
	_body->SetTransform(b2Vec2(setPosition.x, setPosition.y), setAngle);
}

bool RigidBody::allowSleep() const
{
	return _body->IsSleepingAllowed();
}

void RigidBody::allowSleep(bool setAllowSleep)
{
	_body->SetSleepingAllowed(setAllowSleep);
}

bool RigidBody::active() const
{
	return _body->IsActive();
}

void RigidBody::active(bool setActive)
{
	_body->SetActive(setActive);
}

float RigidBody::linearDampening() const
{
	return _body->GetLinearDamping();
}

void RigidBody::linearDampening(float setLinearDampening)
{
	_body->SetLinearDamping(setLinearDampening);
}

float RigidBody::angularDampening() const
{
	return _body->GetAngularDamping();
}

void RigidBody::angularDampening(float setAngularDampening)
{
	_body->SetAngularDamping(setAngularDampening);
}

vec2f RigidBody::velocity() const
{
	auto vel = _body->GetLinearVelocity();
	return vec2f(vel.x, vel.y);
}

void RigidBody::velocity(const vec2f &setVelocity) const
{
	_body->SetLinearVelocity(b2Vec2(setVelocity.x, setVelocity.y));
}

float RigidBody::angularVelocity() const
{
	return _body->GetAngularVelocity();
}

void RigidBody::angularVelocity(float setAngularVelocity) const
{
	_body->SetAngularVelocity(setAngularVelocity);
}

float RigidBody::friction() const
{
	return _body->GetFixtureList()->GetFriction();
}

void RigidBody::friction(float setFriction) const
{
	for(auto fix = _body->GetFixtureList();
		fix != nullptr;
		fix = fix->GetNext())
	{
		fix->SetFriction(setFriction);
	}
}

vec2f RigidBody::right() const
{
	return (vec2f&)_body->GetWorldVector(b2Vec2(-1, 0));
}

vec2f RigidBody::left() const
{
	return (vec2f&)_body->GetWorldVector(b2Vec2(1, 0));
}

vec2f RigidBody::up() const
{ 
	return (vec2f&)_body->GetWorldVector(b2Vec2(0, -1));
}

vec2f RigidBody::down() const
{
	return (vec2f&)_body->GetWorldVector(b2Vec2(0, 1));
}

ContactMask RigidBody::self_mask() const
{
	return _self;
}

void RigidBody::self_mask(ContactMask setMask)
{
	_self = setMask;

	for(auto f = _body->GetFixtureList();
		f != nullptr; f = f->GetNext())
	{
		auto filter = f->GetFilterData();
		filter.categoryBits = (uint16_t)_self;
		f->SetFilterData(filter);
	}
}

ContactMask RigidBody::others_mask() const
{
	return _others;
}

void RigidBody::others_mask(ContactMask setMask)
{
	_others = setMask;

	for(auto f = _body->GetFixtureList();
		f != nullptr; f = f->GetNext())
	{
		auto filter = f->GetFilterData();
		filter.maskBits = (uint16_t)_others;
		f->SetFilterData(filter);
	}
}

b2Body *RigidBody::b2_Body()
{
	return _body;
}

const b2Body *RigidBody::b2_Body() const
{
	return _body;
}

void RigidBody::AddForce(const vec2f &force, const vec2f &point)
{
	_body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y));
}

void RigidBody::AddImpulse(const vec2f &impulse, const vec2f &point)
{
	_body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y));
}

void RigidBody::AddTorque(float torque)
{
	_body->ApplyTorque(torque);
}

void RigidBody::AddAngularImpulse(float impulse)
{
	_body->ApplyAngularImpulse(impulse);
}

void RigidBody::AddCircle(const vec2f &localCenter, 
						  float radius,
						  float scale,
						  float density,
						  float fric,
						  float bounce)
{
	b2CircleShape circle;
	circle.m_radius = radius * scale;
	circle.m_p.Set(localCenter.x * scale,
				   localCenter.y * scale);
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circle;
	fixtureDef.density = density;
	fixtureDef.friction = fric;
	fixtureDef.restitution = bounce;
	fixtureDef.userData = nullptr;
	fixtureDef.filter.categoryBits = (uint16_t)_self;
	fixtureDef.filter.maskBits = (uint16_t)_others;
	fixtureDef.isSensor = (_type == Type::Trigger);

	_body->CreateFixture(&fixtureDef);
}

void RigidBody::AddPolygon(const vec2f *pts,
						   uint32_t count,
						   float scale,
						   float density,
						   float fric,
						   float bounce)
{
	vector<b2Vec2> b2pts;
	b2pts.reserve(count);

	for(uint32_t i = 0; i < count; ++i)
	{
		b2pts.emplace_back(pts[i].x * scale,
						   pts[i].y * scale);
	}

	b2PolygonShape polygon;
	polygon.Set(b2pts.data(), count);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polygon;
	fixtureDef.density = density;
	fixtureDef.friction = fric;
	fixtureDef.restitution = bounce;
	fixtureDef.userData = nullptr;
	fixtureDef.filter.categoryBits = (uint16_t)_self;
	fixtureDef.filter.maskBits = (uint16_t)_others;
	fixtureDef.isSensor = (_type == Type::Trigger);

	_body->CreateFixture(&fixtureDef);
}

void RigidBody::AddBox(float halfWidth,
					   float halfHeight,
					   const vec2f &localCenter,
					   float angle,
					   float scale,
					   float density,
					   float fric,
					   float bounce)
{
	b2PolygonShape box;
	box.SetAsBox(halfWidth * scale,
				 halfHeight * scale,
				 (b2Vec2&)(localCenter * scale),
				 angle);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = density;
	fixtureDef.friction = fric;
	fixtureDef.restitution = bounce;
	fixtureDef.userData = nullptr;
	fixtureDef.filter.categoryBits = (uint16_t)_self;
	fixtureDef.filter.maskBits = (uint16_t)_others;
	fixtureDef.isSensor = (_type == Type::Trigger);

	_body->CreateFixture(&fixtureDef);
}

void RigidBody::AddEdge(const vec2f &p1,
						const vec2f &p2,
						float scale,
						float density,
						float fric,
						float bounce)
{
	b2EdgeShape edge;

	edge.Set(b2Vec2(p1.x * scale, p1.y * scale),
			 b2Vec2(p2.x * scale, p2.y * scale));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &edge;
	fixtureDef.density = density;
	fixtureDef.friction = fric;
	fixtureDef.restitution = bounce;
	fixtureDef.userData = nullptr;
	fixtureDef.filter.categoryBits = (uint16_t)_self;
	fixtureDef.filter.maskBits = (uint16_t)_others;
	fixtureDef.isSensor = (_type == Type::Trigger);

	_body->CreateFixture(&fixtureDef);
}
