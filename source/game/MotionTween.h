/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Math.h"
#include <list>
#include <deque>
#include "Object.h"

using namespace std;

class MotionTween : public Object
{
private:
	deque<vec2f> waypoints;
	deque<vec2f>::iterator itTarget;

	float speed;
	vec2f direction;
	vec2f start, target;
	vec2f position;

	float t;
	float length;
	bool _paused;

	void (MotionTween::*Updater)();
	
	void UpdateLoop();
	void UpdatePath();

public:

	MotionTween();
	MotionTween(const deque<vec2f> &loop, float speed);
	MotionTween(vec2f &startPos, const deque<vec2f> &path, float speed);
	~MotionTween();

	virtual void Update();

	void SetLoop(const deque<vec2f> &loop, float speed);
	void SetPath(vec2f &startPos, const deque<vec2f> &path, float speed);
	
	void paused(bool setPaused);
	bool paused() const;

	void Clear();

	bool Empty();
	const vec2f &GetPosition();
	const vec2f &GetDirection();
};