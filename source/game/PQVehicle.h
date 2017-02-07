/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <Box2D.h>
#include "PQB2Shapes.h"
#include "PQCharacter.h"
#include "Object.h"
#include "PQGameTypes.h"
#include <memory>

class PQGame;

/////////CAR STATES//////////

class PQVehicle : public PQCharacter
{
public:
	PQVehicle();
	virtual ~PQVehicle();

	vec2f input;
	float degreesPerCol;

	virtual void Update();
	virtual void LateUpdate();

	virtual string tag() const override { return "Vehicle"; }

	void Freeze();
	void Unfreeze();
	void DoBoost();
	void KillBoost();
	bool InUse();

	virtual void Start() override;
	virtual void OnCollisionEnter(Contact contact) override;

	float maxFrontSpeed;
	float accelleration;
	float sideFriction;
	float turnTorque;
	
private:
	bool inUse;
	bool destroyed;
	float nAngles;
	float angleTolerance;
	float carAngle;
	bool hasBoost;
	float boostFinish;
	float boostLength;
	weak_ptr<Task> boostRoutine;
};
