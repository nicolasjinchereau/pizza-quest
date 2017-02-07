/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQCopCar.h"
#include "PQGame.h"
#include "Engine.h"
#include "PizzaQuest.h"

PQCopCar::PQCopCar(PQGame *game)
	: PQPathFinder(&game->vehGraph, &game->vehGoals, 120.0f)
{
	currentAngle = 0;
	newAngle = 0;
	angleSpeedFactor = 3.0f;
	alive = true;

	lastDir = vec2f::right;
	_destroyed = false;
}

PQCopCar::~PQCopCar()
{

}

void PQCopCar::Start()
{
	PQPathFinder::Start();

	img->layer = DrawLayer::Cars;

	body = make_shared<RigidBody>(state()->physics,
								  GetResource()->collision_shapes,
								  position,
								  math::rad(angle),
								  scale);

	body->type(RigidBody::Type::Kinematic);
	body->self_mask(ContactMask::Car);
	body->others_mask(ContactMask::Vehicle);

	AddChild(body);

	nAngles = img->GetNumCols();
	float degreesPerCol = 360.0f / (float)nAngles;
	angleTolerance = degreesPerCol / 2.0f;
	
	float ang = 0.0f;

	spriteAngles.reserve(nAngles);
	
	for(int i = 0; i < nAngles; i++)
	{
		spriteAngles.push_back(ang);
		ang += degreesPerCol;
	}
}

void PQCopCar::LateUpdate()
{
	PQPathFinder::LateUpdate();

	if(!tween->Empty())
	{
		vec2f dir = GetDirection();
	
		newAngle = atan2(dir.y, dir.x);
		newAngle = math::mod(newAngle, math::twopi);

		float da = Time::deltaTime() * angleSpeedFactor;
		currentAngle = math::lerp_angle(currentAngle, newAngle, da);
	}

	rightOffset = vec2f(cosf(currentAngle), sinf(currentAngle)).PerpCW();
	rightOffset *= 22.0f;
	
	img->SetPos(position + rightOffset);

	vec2f pos = Physics::toMeters(position + rightOffset);
	
	body->position(pos);
	body->transform(pos, currentAngle);
	
	float degAngle = math::mod(-math::deg(currentAngle), 360.0f);

	if(degAngle >= 360 - angleTolerance)
		degAngle -= 360.0f;
	
	for(int i = 0; i < nAngles; i++)
	{
		if(degAngle >= spriteAngles[i] - angleTolerance
		&& degAngle <= spriteAngles[i] + angleTolerance)
		{
			img->SetColumn(i);
			break;
		}
	}
}

property<bool> PQCopCar::destroyed()
{
	struct property
	{
		PQCopCar *object;

		bool get() {
			return object->_destroyed;
		}

		void set(bool value) {
			object->_destroyed = value;
			object->LeavePath();
			object->img->SetTexture(PizzaQuest::textures().burnCopCar);
		}
	};

	return property{this};
}
