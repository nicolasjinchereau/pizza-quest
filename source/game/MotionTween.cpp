/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "MotionTween.h"
#include "Engine.h"
#include "Time.h"

MotionTween::MotionTween()
{
	t = 0.0f;
	length = 0.0f;
	direction = vec2f::zero;
	speed = 0.0f;
	_paused = false;

	Updater = &MotionTween::UpdatePath;
}

MotionTween::MotionTween(const deque<vec2f> &loop, float speed)
{
	SetLoop(loop, speed);
}

MotionTween::MotionTween(vec2f &startPos, const deque<vec2f> &path, float speed)
{
	SetPath(startPos, path, speed);
}

void MotionTween::SetLoop(const deque<vec2f> &loop, float speed)
{
	if(loop.size() < 3)
		return;

	waypoints = loop;
	this->speed = speed;

	itTarget = waypoints.begin();
	start = (*itTarget++);
	target = (*itTarget);

	direction = target - start;
	length = direction.Length();
	direction /= length;

	t = 0.0f;
	position = start;
	_paused = false;

	Updater = &MotionTween::UpdateLoop;
}

void MotionTween::SetPath(vec2f &startPos, const deque<vec2f> &path, float speed)
{
	if(path.size() < 1)
		return;

	start = startPos;
	waypoints = path;
	this->speed = speed;

	direction = waypoints.front() - startPos;
	length = direction.Length();
	direction /= length;

	t = 0.0f;
	position = start;
	_paused = false;

	Updater = &MotionTween::UpdatePath;
}

MotionTween::~MotionTween()
{

}

void MotionTween::Update()
{
	if(waypoints.empty() || _paused)
		return;
	
	(this->*Updater)();
}

void MotionTween::UpdateLoop()
{
	// move along the path
	t += speed * Time::deltaTime();

	// if past the end of the current trajectory,
	if(t >= length)
	{
		t -= length;

		// move the start point up to the next waypoint
		start = target;

		// loop around if past the end
		if((++itTarget) == waypoints.end())
			itTarget = waypoints.begin();
		
		// store the new target
		target = (*itTarget);

		// create a new trajectory
		direction = target - start;
		length = direction.Length();
		direction /= length;
	}
	
	// output the position
	position = start + (direction * t);
}

void MotionTween::UpdatePath()
{
	// move along the path
	t += speed * Time::deltaTime();

	// if past the end of the current trajectory,
	if(t >= length)
	{
		// move the start point up to the next waypoint
		start = waypoints.front();
		waypoints.pop_front();

		// if there are any more waypoints,
		if( !waypoints.empty() )
		{
			// create a new trajectory
			t -= length;
			direction = waypoints.front() - start;
			length = direction.Length();
			direction /= length;
		}
		else
		{
			// stop at end
			direction = vec2f::zero;
			length = 0.0f;
			t = 0.0f;
		}
	}
	
	// output the position
	position = start + (direction * t);
}

void MotionTween::paused(bool setPaused)
{
	_paused = setPaused;
}

bool MotionTween::paused() const
{
	return _paused;
}

void MotionTween::Clear()
{
	waypoints.clear();
	_paused = false;
}

bool MotionTween::Empty()
{
	return waypoints.empty();
}

const vec2f &MotionTween::GetPosition()
{
	return position;
}

const vec2f &MotionTween::GetDirection()
{
	return direction;
}
