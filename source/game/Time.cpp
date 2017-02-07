/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Time.h"
#include <windows.h>
#include "Trace.h"

Time::Time(int animation_fps, int max_fps)
{
	_frequency = frequency();
	_initTime = ticks();

	_time = 0;
	_deltaTime = 0;
	_fps = 0;
	_then = 0;
	_lastFpsCalc = 0;
	_elapsedFrames = 0;
	_pauseTime = 0;
	_paused = false;
	_minFrameLength = _frequency / (long long)max_fps;
}

Time::~Time()
{
	
}

void Time::Step()
{
	Time *t = that;

	long long _now = ticks() - t->_initTime;
	long long _elapsed = _now - t->_then;
	
	t->_then = _now;
	t->_time = (float)((double)_now / (double)t->_frequency);
	t->_deltaTime = (float)((double)_elapsed / (double)t->_frequency);

	++t->_elapsedFrames;

	if(_now - t->_lastFpsCalc > t->_frequency)
	{
		t->_fps = ++t->_elapsedFrames;
		t->_elapsedFrames = 0;
		t->_lastFpsCalc = _now;
	}
}

void Time::Pause()
{
	Time *t = that;

	t->_pauseTime = ticks();
	t->_paused = true;
}

void Time::Resume()
{
	Time *t = that;

	if(t->_paused)
	{
		long long _now = ticks();

		long long timePaused = (_now - t->_pauseTime);
		t->_initTime += timePaused;
	}
	else
	{
		Reset();
	}
}

void Time::Sleep(float seconds)
{
	::Sleep((DWORD)(seconds * 1000.0f));
}

void Time::Reset()
{
	Time *t = that;

	t->_initTime = ticks();
	t->_time = 0;
	t->_deltaTime = 0;
	t->_fps = 0;
	t->_then = 0;
	t->_lastFpsCalc = 0;
	t->_elapsedFrames = 0;
	t->_pauseTime = 0;
	t->_paused = false;
}

float Time::time()
{
	return that->_time;
}

float Time::exactTime()
{
	Time *t = that;
	long long now = t->ticks() - t->_initTime;
	return (float)((double)now / (double)t->_frequency);
}

float Time::deltaTime()
{
	return that->_deltaTime;
}

int Time::fps()
{
	return that->_fps;
}

long long Time::ticks()
{
	long long ret;
	QueryPerformanceCounter((LARGE_INTEGER*)&ret);
	return ret;
}

long long Time::frequency()
{
	long long ret;
	QueryPerformanceFrequency((LARGE_INTEGER*)&ret);
	return ret;
}
