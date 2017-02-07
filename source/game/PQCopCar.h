/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Sprite.h"
#include "Animation.h"
#include "Trace.h"
#include "PQPathFinder.h"
#include "property.h"

class PQGame;

class PQCopCar : public PQPathFinder
{
	float angleTolerance;
	int nAngles;
	vector<float> spriteAngles;
	
	bool _destroyed;

public:
	vec2f rightOffset;
	float currentAngle;
	float newAngle;
	float delta;
	float angleSpeedFactor;
	bool alive;
	vec2f lastDir;

	PQCopCar(PQGame *game);
	virtual ~PQCopCar();
	
	virtual void LateUpdate();
	virtual string tag() const override { return "Car"; }

	virtual void Start() override;

	property<bool> destroyed();
};
