/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"
#include "Math.h"
#include "Sprite.h"
#include "Trace.h"
#include "Animation.h"
#include "Time.h"
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include "Camera.h"


using namespace std;

class PQGame;

class PQGameTimer : public Object
{
public:
	enum TIMER_STATE
	{
		TS_STOPPED,
		TS_PAUSED,
		TS_RUNNING,
	};

	shared_ptr<Sprite> numbers;
	shared_ptr<Sprite> background;

	PQGameTimer();
	~PQGameTimer();

	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;

	void SetTimeLimit(float limit);
	void AddTime(float length);

	void StartTimer();
	void PauseTimer();
	void StopTimer();

	virtual void Start() override;
	virtual void Draw() override;

	float timeLimit();
	float timeElapsed();
	float timeRemaining();
	bool timedOut();

	void GetLoadTasks(vector<function<void()>> &loadTasks);

private:
	float _timeLimit;
	float _timeElapsed;
	float _timeRemaining;
	bool _timedOut;

	vec2f _normPos;
	int _timerState;
	
	float _startTime;
	float _pauseStarted;
	float _systemPauseStarted;

	virtual void Update();
};
