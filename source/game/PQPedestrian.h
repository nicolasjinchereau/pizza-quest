/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Sprite.h"
#include "Animation.h"
#include "PQGame.h"
#include "Trace.h"
#include "PQPathFinder.h"

class PQPedestrian : public PQPathFinder
{
public:
	int spriteRow;
	float angleTolerance;
	static const int nAngles = 8;
	float angles[nAngles];
	shared_ptr<Animation> animations[nAngles];
	Animation *animation;
	shared_ptr<Sprite> splatter;
	bool isSplattered;

	PQPedestrian(PQGame *game);
	virtual ~PQPedestrian();
	
	virtual void Start() override;
	virtual void LateUpdate();
	virtual void Draw() override;
	virtual string tag() const override { return "Pedestrian"; }
	virtual void OnCollisionEnter(Contact contact) override;

	void SetSplattered();

	b2PolygonShape feelerShape;
};
