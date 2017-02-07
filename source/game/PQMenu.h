/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Engine.h"
#include "State.h"
#include "Sprite.h"
#include "Camera.h"
#include "Sound.h"
#include "Time.h"
#include "Button.h"
#include "Sound.h"
#include <memory>

class PQMenu : public State
{
public:
	PQMenu();
	virtual ~PQMenu();

	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTouchDown(float x, float y, int id) override;
	virtual void OnTouchUp(float x, float y, int id) override;
	virtual void OnKeyUp(KeyCode keyCode) override;
	void DoButtonClick(function<void()> then);

private:

	void Continue();
	void NewGame();
	void Credits();
	void Quit();

	bool quit;
	bool didClick;
	weak_ptr<Task> swipeInRoutine;
	shared_ptr<Camera> camera;
	shared_ptr<Sprite> background;
	shared_ptr<Button> btnContinue;
	shared_ptr<Button> btnNew;
	shared_ptr<Button> btnCredits;
	shared_ptr<Button> btnQuit;
};
