/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "State.h"
#include "Sprite.h"
#include "Button.h"
#include "Camera.h"
#include "PlayerProfile.h"
#include "Engine.h"
#include <memory>

class PQScoreScreen : public State
{
public:

	shared_ptr<Camera> camera;
	shared_ptr<Sprite> scoreBackground;
	shared_ptr<Button> btnQuit;
	shared_ptr<Button> btnNext;
	shared_ptr<Button> btnRetry;
	
	PQScoreScreen();
	virtual ~PQScoreScreen();

	void Quit();
	void Retry();
	void Next();
	
	virtual void Start() override;
	virtual void Update() override;

	//virtual void Draw();
	virtual void OnTouchDown(float x, float y, int id);
	virtual void OnTouchUp(float x, float y, int id);
	virtual void OnBackPressed();
};
