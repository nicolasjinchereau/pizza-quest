/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Math.h"
#include "Sprite.h"
#include "Trace.h"
#include "Object.h"
#include "Time.h"
#include <functional>


using namespace std;

class PQGame;

class Button : public Object
{
public:
	Button();
	virtual ~Button();

	bool Open(const char *filename);

	bool TryPress(float wx, float wy, int id);
	bool TryRelease(int id);
	void ForceRelease();
	bool IsDown();

	virtual void Update() override;
	virtual void PreDrawUpdate() override;

	void SetPressEvent(const function<void()> &fn);
	void SetReleaseEvent(const function<void()> &fn);

	void SetPosition(float xn, float yn);
	vec2f position() const;

	shared_ptr<Sprite> image();
	void image(const shared_ptr<Sprite> &setImage);

protected:
	function<void()> onButtonDown;
	function<void()> onButtonUp;
	
	float buttonClicked;
	int pID;

	bool down;
	vec2f normalizedPosition;
	shared_ptr<Sprite> _image;
};

