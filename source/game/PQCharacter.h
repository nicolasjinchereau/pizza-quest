/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <string>
using namespace std;

#include "Math.h"
#include "Object.h"
#include "PQGameTypes.h"
#include "Sprite.h"
#include "Animation.h"
#include "Sound.h"
#include "Shader.h"
#include "Trace.h"

class PQGame;

class PQCharacter : public PQMapImage
{
public:
	int health;

	PQCharacter();
	virtual ~PQCharacter();
	virtual string tag() const override { return "Character"; }
};
