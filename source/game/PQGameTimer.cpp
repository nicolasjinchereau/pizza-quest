/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQGameTimer.h"
#include "PQGame.h"
#include <cassert>
using namespace std;

PQGameTimer::PQGameTimer()
{
	category = 2;
	layer = DrawLayer::UserInterface;

	_normPos = vec2f::zero;
	_startTime = 0;
	_pauseStarted = 0;
	_systemPauseStarted = 0;
	_timerState = TS_STOPPED;

	_timeLimit = 0;
	_timeElapsed = 0;
	_timeRemaining = 0;
	_timedOut = false;
}

PQGameTimer::~PQGameTimer()
{

}

void PQGameTimer::normalizedPosition(const vec2f &setPosition)
{
	_normPos = setPosition;
}

vec2f PQGameTimer::normalizedPosition() const
{
	return _normPos;
}

void PQGameTimer::StartTimer()
{
	if(_timerState == TS_PAUSED)
	{
		_startTime += (Time::time() - _pauseStarted);
	}
	else
	{
		_timeElapsed = 0;
		_timeRemaining = _timeLimit;	
		_startTime = Time::time();
	}

	_timedOut = false;
	_timerState = TS_RUNNING;
}

void PQGameTimer::PauseTimer()
{
	if(_timerState == TS_RUNNING)
	{
		_timerState = TS_PAUSED;
		_pauseStarted = Time::time();
	}
}

void PQGameTimer::StopTimer()
{
	_timedOut = false;
	_timeElapsed = 0;
	_timeRemaining = 0;
	_timerState = TS_STOPPED;
}

void PQGameTimer::SetTimeLimit(float limit)
{
	_timeLimit = limit;
}

void PQGameTimer::AddTime(float length)
{
	_startTime += length;
}

void PQGameTimer::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		numbers = AddChild(make_shared<Sprite>());
		numbers->Open("assets\\Images\\GUI\\GameTimer.png");
		numbers->SetNumCols(11);
		numbers->SetNumRows(2);
		numbers->category = 0;
	});

	loadTasks.emplace_back([this]{
		background = AddChild(make_shared<Sprite>());
		background->Open("assets\\Images\\GUI\\TimerBackground.png");
		background->category = 0;
	});
}

void PQGameTimer::Start()
{

}

void PQGameTimer::Update()
{
	if(_timerState == TS_RUNNING)
	{
		_timeElapsed = Time::time() - _startTime;
		_timeRemaining = _timeLimit - _timeElapsed;

		if(_timeRemaining <= 0)
		{
			_timeRemaining = 0;
			_timeElapsed = _timeLimit;
			_timerState = TS_STOPPED;
			_timedOut = true;
		}
	}
}

void PQGameTimer::Draw()
{
	auto game = state()->as<PQGame>();
	vec2f position = game->guiCamera->NormalizedToWorld(_normPos);

	int totalSecRemaining = (int)_timeRemaining;

	int minutes = totalSecRemaining / 60;
	int seconds = totalSecRemaining - (minutes * 60);

	int mins_a = minutes < 10 ? 0 : minutes / 10;
	int mins_b = minutes - mins_a * 10;
	
	int secs_a = seconds < 10 ? 0 : seconds / 10;
	int secs_b = seconds - secs_a * 10;

	float characterWidth = (float)numbers->GetWidth();
	float xStart = position.x - (2.0f * characterWidth);
	
	background->SetPos(position);
	background->Draw();

	numbers->SetPos(position);
	numbers->SetRow((totalSecRemaining > 10) ? 0 : 1);
	
	numbers->SetX(xStart);
	xStart += characterWidth;
	numbers->SetColumn(mins_a);
	numbers->Draw();
	
	numbers->SetX(xStart);
	xStart += characterWidth;
	numbers->SetColumn(mins_b);
	numbers->Draw();
	
	numbers->SetX(xStart);
	xStart += characterWidth;
	numbers->SetColumn(10);
	numbers->Draw();
	
	numbers->SetX(xStart);
	xStart += characterWidth;
	numbers->SetColumn(secs_a);
	numbers->Draw();
	
	numbers->SetX(xStart);
	xStart += characterWidth;
	numbers->SetColumn(secs_b);
	numbers->Draw();
}

float PQGameTimer::timeLimit()
{
	return _timeLimit;
}

float PQGameTimer::timeElapsed()
{
	return _timeElapsed;
}

float PQGameTimer::timeRemaining()
{
	return _timeRemaining;
}

bool PQGameTimer::timedOut()
{
	return _timedOut;
}
