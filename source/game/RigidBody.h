/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include "Object.h"
#include "PQB2Shapes.h"
#include "EnumBitmask.h"

class b2Body;
class b2World;
class Physics;

enum class ContactMask : uint16_t
{
	All = UINT16_MAX,
	None = 0,
	PowerUp = 1,
	Vehicle = 1 << 1,
	Car = 1 << 2,
	Pedestrian = 1 << 3,
	Player = 1 << 4,
	Delivery = 1 << 5,
	Pickup = 1 << 6,
	Tile = 1 << 7,
	Prop = 1 << 8,
	Structure = 1 << 9,
	MapBorder = 1 << 10,
};

ENUM_BITMASK(ContactMask)

class RigidBody : public Object
{
	RigidBody(const RigidBody &other){}
	RigidBody& operator=(const RigidBody &other) { return *this; }

	weak_ptr<Physics> _physics;

public:
	enum class Type
	{
		Dead = -1,
		Static,
		Dynamic,
		Kinematic,
		Trigger,
	};

	RigidBody();
	RigidBody(RigidBody &&other);
	RigidBody& operator=(RigidBody &&other);
	RigidBody(shared_ptr<Physics> physics,
			  const vector<shared_ptr<PQB2Shape>> &shapes,
			  const vec2f &position,
			  float angle,
			  float scale,
			  Type type = Type::Static,
			  ContactMask self = ContactMask::All,
			  ContactMask others = ContactMask::All);

	RigidBody(shared_ptr<Physics> physics,
			  Type type = Type::Static,
			  ContactMask self = ContactMask::All,
			  ContactMask others = ContactMask::All);

	RigidBody(shared_ptr<Physics> physics,
			  const vec2f &position,
			  float angle,
			  Type type = Type::Static,
			  ContactMask self = ContactMask::All,
			  ContactMask others = ContactMask::All);

	~RigidBody();

	b2Body *b2_Body();
	const b2Body *b2_Body() const;
	Type type() const;
	void type(Type setType);
	Object *owner();
	vec2f position() const;
	vec2f  pixelPosition() const;
	void pixelPosition(const vec2f &setPixelPosition);
	void position(const vec2f &setPosition);
	float angle() const;
	void angle(float setAngle);
	float mass() const;
	void mass(float setMass);
	float inertia() const;
	void transform(const vec2f &setPosition, float setAngle);
	bool allowSleep() const;
	void allowSleep(bool setAllowSleep);
	bool active() const;
	void active(bool setActive);
	float linearDampening() const;
	void linearDampening(float setLinearDampening);
	float angularDampening() const;
	void angularDampening(float setAngularDampening);
	vec2f velocity() const;
	void velocity(const vec2f &setVelocity) const;
	float angularVelocity() const;
	void angularVelocity(float setFriction) const;
	float friction() const;
	void friction(float setAngularVelocity) const;
	vec2f right() const;
	vec2f left() const;
	vec2f up() const;
	vec2f down() const;

	ContactMask self_mask() const;
	void self_mask(ContactMask setMask);
	ContactMask others_mask() const;
	void others_mask(ContactMask setMask);
	

	void AddForce(const vec2f &force, const vec2f &point);
	void AddImpulse(const vec2f &impulse, const vec2f &point);
	void AddTorque(float torque);
	void AddAngularImpulse(float impulse);

	void AddCircle(const vec2f &localCenter,
				   float radius,
				   float scale = 1,
				   float density = 0.1f,
				   float fric = 0.2f,
				   float bounce = 0);

	void AddPolygon(const vec2f *pts,
					uint32_t count,
					float scale = 1,
					float density = 0.1f,
				    float fric = 0.2f,
				    float bounce = 0);

	void AddBox(float halfWidth,
				float halfHeight,
				const vec2f &localCenter,
				float angle,
				float scale = 1,
				float density = 0.1f,
				float fric = 0.2f,
				float bounce = 0);

	void AddEdge(const vec2f &p1,
				 const vec2f &p2,
				 float scale = 1,
				 float density = 0.1f,
				 float fric = 0.2f,
				 float bounce = 0);

private:
	b2Body *_body;
	ContactMask _self;
	ContactMask _others;
	Type _type;
};
