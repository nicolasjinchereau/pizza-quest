/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <cstdlib>
#include "Singleton.h"

class Time : public Singleton<Time>
{
	long long _frequency;
	long long _minFrameLength;
	long long _animFrameLength;
	long long _initTime;
	long long _pauseTime;
	bool _paused;

	long long _then;
	float _time;
	float _deltaTime;
	int _fps;
	
	long long _lastFpsCalc;
	int _elapsedFrames;
public:

	Time(int animation_fps = 16, int max_fps = 90);
	~Time();
	
	static void Step();
	static void Pause();
	static void Resume();
	static void Sleep(float seconds);
	static void Reset();
	
	static float time();
	static float exactTime();
	static float deltaTime();
	static int fps();
	static long long ticks();
	static long long frequency();
};

