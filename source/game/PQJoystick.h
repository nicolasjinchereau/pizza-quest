/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"
#include "Sprite.h"
#include "Trace.h"
#include "Camera.h"


class PQGame;

class PQJoystick : public Object
{
public:
	PQJoystick(float wx = 0, float wy = 0);
	~PQJoystick();

	// thumbpad will start to move with thumb
	// succeeds if thumb is closer to center of pad than activeRadius
	void Press(float wx, float wy, int id);
	void SetThumbPos(float wx, float wy, int id);
	void TryRelease(int id);
	void ForceRelease();

	// thumb vec is clamped to (0.0f to 1.0f) as percent of maxRadius
	const vec2f &GetThumbVec();
	
	virtual void Start() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);

private:

	bool active;
	int pID;

	vec2f position;
	vec2f thumb;
	vec2f clampedThumb;
	float radius;
	float length;

	shared_ptr<Sprite> center;
	shared_ptr<Sprite> ring;
};
