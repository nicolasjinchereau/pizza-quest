/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"
#include "PQGameTypes.h"
#include "Sprite.h"
#include "Animation.h"
#include "Time.h"
#include "Shader.h"
#include "PQPlayer.h"
#include <Box2D.h>
#include "RigidBody.h"

class PQPizzaPickup : public PQMapImage
{
	shared_ptr<Shader> shader;
public:
	PQPizzaPickup();
	~PQPizzaPickup();

	virtual void Start() override;
	virtual void LateUpdate() override;
	virtual void OnCollisionEnter(Contact contact) override;
	virtual string tag() const override { return "PizzaPickup"; }

	void Enable(bool enable);
};
