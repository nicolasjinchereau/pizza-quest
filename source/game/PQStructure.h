/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "PQGameTypes.h"

class PQStructure : public PQMapImage
{
public:
	PQStructure();
	~PQStructure();

	virtual void Start() override;
	virtual string tag() const override { return "Structure"; }
};
