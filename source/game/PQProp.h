/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "PQGameTypes.h"

class PQProp : public PQMapImage
{
public:
	PQProp();
	~PQProp();

	virtual void Start() override;
	virtual string tag() const override { return "Prop"; }
};
