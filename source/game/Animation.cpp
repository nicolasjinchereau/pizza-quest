/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Animation.h"
#include "Engine.h"
#include "Sprite.h"

Animation::Animation()
{
	currentTime = 0;
	animLength = 0;

	playAutomatically = false;
	playing = false;
	paused = false;
	reverse = false;
	loop = false;
}

Animation::~Animation()
{

}

void Animation::sprite(const shared_ptr<Sprite> &setSprite)
{
	_sprite = setSprite;
}

shared_ptr<Sprite> Animation::sprite() const
{
	return _sprite;
}

void Animation::AddFrame(uint8_t row, uint8_t col)
{
	frames.push_back(Frame(row, col));
	animLength = (float)frames.size() / 16.0f;
}

void Animation::SetTime(float time)
{
	Frame &frame = frames[(int)(time * 16.0f - FLT_EPSILON)];
	_sprite->SetFrame(frame.row, frame.col);
}

int Animation::frameCount()
{
	return (int)frames.size();
}

Animation::Frame &Animation::frame(int index)
{
	return frames[index];
}

void Animation::Draw()
{
	_sprite->Draw();
}

void Animation::Play(bool loop, bool reverse)
{
	if(this->frames.size() > 0)
	{
		if(this->paused)
		{
			this->paused = false;
		}
		else
		{
			this->loop = loop;
			this->playing = true;
			this->currentTime = 0;
			this->reverse = reverse;
		}
	}
}

void Animation::Pause()
{
	paused = true;
}

void Animation::Stop()
{
	playing = false;
	paused = false;
	currentTime = 0;

	if(_sprite)
		_sprite->SetFrame(0, 0);
}

void Animation::Reverse()
{
	reverse = !reverse;
}

void Animation::Goto(int index)
{
	if(_sprite)
	{
		Frame &frame = frames[index];
		_sprite->SetFrame(frame.row, frame.col);
	}
}

bool Animation::IsPlaying()
{
	return playing && !paused;
}

bool Animation::IsPaused()
{
	return paused;
}

bool Animation::IsReversed()
{
	return reverse;
}

bool Animation::IsFinised()
{
	return !playing;
}

void Animation::PreDrawUpdate()
{
	if(_sprite && playing && !paused)
	{
		float dt = reverse ? -Time::deltaTime() : Time::deltaTime();
		currentTime += dt;
		
		if(loop)
		{
			if(currentTime >= animLength)
			{
				currentTime = fmod(currentTime, animLength);
			}
			else if(currentTime < 0.0f)
			{
				currentTime = fmod(currentTime, animLength) + animLength;
			}
		}
		else
		{
			if(currentTime >= animLength)
			{
				currentTime = 0;
				playing = false;
			}
			else if(reverse && currentTime < 0.0f)
			{
				currentTime = 0;
				playing = false;
			}
		}

		SetTime(currentTime);
	}
}
