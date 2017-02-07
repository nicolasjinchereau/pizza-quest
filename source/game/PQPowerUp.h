/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "PQGameTypes.h"

class PQPowerUp : public PQMapImage
{
public:
	enum class Type
	{
		Health,
		Speed,
		Time,
	};

	PQPowerUp(Type type);
	virtual ~PQPowerUp();

	virtual void Start() override;
	virtual void Update() override;
	virtual string tag() const override { return "PowerUp"; }

	virtual void OnCollisionEnter(Contact contact) override;

private:
	Type _type;
	float nextBlink;
};
