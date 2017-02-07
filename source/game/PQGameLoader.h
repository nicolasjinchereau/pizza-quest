/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "State.h"
#include "Camera.h"
#include "Sprite.h"
#include <mutex>
#include "PQProgressBar.h"
#include "Engine.h"
#include <coroutine.h>
using namespace coroutines;

class PQGame;

class PQGameLoader : public State
{
public:
	PQGameLoader();
	virtual ~PQGameLoader();

	virtual void Start() override;
	virtual void Update();
	//virtual void Draw();
	
private:
	
	coroutine<float> loader;

	shared_ptr<Camera> camera;
	shared_ptr<Sprite> background;
	shared_ptr<PQProgressBar> progressBar;
	shared_ptr<PQGame> game;
};
