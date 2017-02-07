/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "Trace.h"
#include "Math.h"
#include "Object.h"
#include <vector>

using namespace std;

class Sprite;
class Camera;

class Animation : public Object
{
	shared_ptr<Sprite> _sprite;
public:
	struct Frame
	{
		Frame(uint8_t row, uint8_t col) : row(row), col(col){}

		uint8_t row;
		uint8_t col;
	};

	Animation();
	~Animation();

	void sprite(const shared_ptr<Sprite> &setSprite);
	shared_ptr<Sprite> sprite() const;

	void AddFrame(uint8_t row, uint8_t col);
	void SetTime(float time);

	int frameCount();
	Frame &frame(int index);

	void Play(bool loop = false, bool reverse = false);
	void Pause();
	void Stop();
	void Reverse();
	void Goto(int index);
	
	bool IsPlaying();
	bool IsPaused();
	bool IsReversed();
	bool IsFinised();

	virtual void PreDrawUpdate() override;
	virtual void Draw() override;

private:
	
	bool playAutomatically;
	bool playing;
	bool paused;
	bool reverse;
	bool loop;

	vector<Frame> frames;
	float animLength;
	float currentTime;
};