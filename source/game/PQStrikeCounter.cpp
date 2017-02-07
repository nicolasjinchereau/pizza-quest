/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQStrikeCounter.h"
#include "State.h"
#include "PQGame.h"

PQStrikeCounter::PQStrikeCounter()
{
	category = 2;
	layer = DrawLayer::UserInterface;

	_normPos = vec2f::zero;
	_visible = true;
	_strikes = 0;
	_scale = 0.375f;
}

PQStrikeCounter::~PQStrikeCounter()
{

}

void PQStrikeCounter::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		strikeTexture = make_shared<Texture>("assets\\Images\\GUI\\gavel.png");
		strikeImage = AddChild(make_shared<Sprite>());
		strikeImage->SetTexture(strikeTexture);
		strikeImage->SetScale(_scale);
		strikeImage->category = 0;
	});
	
	loadTasks.emplace_back([this]{
		strikeImageGray = AddChild(make_shared<Sprite>());
		strikeImageGray->Open("assets\\Images\\GUI\\gavelGray.png");
		strikeImageGray->SetScale(_scale);
		strikeImageGray->category = 0;
	});

	loadTasks.emplace_back([this]{
		gavelStrike = AddChild(make_shared<Sound>());
		gavelStrike->Open("assets\\Sounds\\Effects\\gavel.wav");
	});
}

void PQStrikeCounter::Draw()
{
	if(_visible)
	{
		auto game = state()->as<PQGame>();
		
		vec2f pos = game->guiCamera->NormalizedToWorld(_normPos);
		
		for(uint32_t i = 0; i < 3; ++i)
		{
			if(i < _strikes)
			{
				strikeImage->SetPos(pos);
				strikeImage->Draw();
			}
			else
			{
				strikeImageGray->SetPos(pos);
				strikeImageGray->Draw();
			}

			pos.x += strikeImage->GetWidth() * _scale + 5.0f;
		}
	}
}

void PQStrikeCounter::normalizedPosition(const vec2f &setPosition)
{
	_normPos = setPosition;
}

vec2f PQStrikeCounter::normalizedPosition() const
{
	return _normPos;
}

void PQStrikeCounter::strikeCount(uint32_t setStrikeCount)
{
	_strikes = setStrikeCount;
}

uint32_t PQStrikeCounter::strikeCount() const
{
	return _strikes;
}

vec2f PQStrikeCounter::strikePos(uint32_t index)
{
	auto game = state()->as<PQGame>();
	 
	vec2f ret = game->guiCamera->NormalizedToWorld(_normPos);
	ret.x += index * strikeImage->GetWidth() * _scale + 5.0f;

	return ret;
}

void PQStrikeCounter::AddStrike()
{
	if(_strikes == 3)
		return;

	RunCoroutine([this](yield_token<float> yield)
	{
		auto game = state()->as<PQGame>();

		shared_ptr<Sprite> strike = AddChild(make_shared<Sprite>(strikeTexture));
		strike->category = 2;
		strike->layer = DrawLayer::UserInterface;

		vec2f startPos = game->guiCamera->NormalizedToWorld(vec2f(0.5f, 0.5f));

		strike->SetPos(startPos);
		strike->SetScale(0);

		float length = 0.3f;
		float start = Time::time();
		float finish = start + length;

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);
			strike->SetScale(2.0f * elapsed);

			yield(0);
		}

		strike->SetScale(2.0f);
		gavelStrike->Play();
		
		yield(1.0f);

		start = Time::time();
		finish = start + length;
		
		vec2f endPos = strikePos(_strikes);

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);
			
			float scale = math::lerp(2.0f, _scale, elapsed);
			vec2f pos = math::lerp(startPos, endPos, elapsed);

			strike->SetScale(scale);
			strike->SetPos(pos);

			yield(0);
		}

		RemoveChild(strike);

		++_strikes;
	});
}
