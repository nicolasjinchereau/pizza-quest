/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <memory>
#include "State.h"
#include "Sprite.h"
#include "Camera.h"
#include "Sound.h"
#include "Time.h"
#include "Button.h"
#include "Engine.h"

class PQGame;

class PQPauseMenu : public State
{
public:
	PQPauseMenu(shared_ptr<PQGame> game);
	virtual ~PQPauseMenu();

	virtual void Start() override;
	//virtual void Draw();

	virtual void OnTouchDown(float x, float y, int id);
	virtual void OnTouchUp(float x, float y, int id);


private:
	void Quit();
	void Retry();
	void Resume();

	shared_ptr<Camera> camera;
	shared_ptr<Sprite> background;
	shared_ptr<Button> btnRetry;
	shared_ptr<Button> btnQuit;
	shared_ptr<Button> btnResume;
	shared_ptr<PQGame> game;
	float oldSongGain;
};

