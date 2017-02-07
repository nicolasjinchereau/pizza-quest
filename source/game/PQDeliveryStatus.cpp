/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQDeliveryStatus.h"
#include "State.h"
#include "PQGame.h"
#include "curves.h"

PQDeliveryStatus::PQDeliveryStatus()
{
	category = 2;
	layer = DrawLayer::UserInterface;

	_normPos = vec2f::zero;
	_visible = true;
	_completed = 0;
	_scale = 0.25f;
}

PQDeliveryStatus::~PQDeliveryStatus()
{

}

void PQDeliveryStatus::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		pizzaIconGray = make_shared<Texture>("assets\\Images\\GUI\\pizzaIconGray.png");
	});

	loadTasks.emplace_back([this]{
		pizzaIconFull = make_shared<Texture>("assets\\Images\\GUI\\pizzaIconFull.png");
	});

	loadTasks.emplace_back([this]{
		pizzaIconChecked = make_shared<Texture>("assets\\Images\\GUI\\pizzaIconChecked.png");
	});
}

void PQDeliveryStatus::Draw()
{
	if(_visible)
	{
		auto game = state()->as<PQGame>();
		
		vec2f pos = game->guiCamera->NormalizedToWorld(_normPos);
		
		for(size_t i = 0, sz = deliveries.size(); i < sz; ++i)
		{
			if(i < _completed)
			{
				deliveries[i]->SetTexture(pizzaIconChecked);
				deliveries[i]->SetScale(_scale);
			}
			else if(i == _completed && game->player->HasPizza())
			{
				deliveries[i]->SetTexture(pizzaIconFull);

				float osc = (sin(Time::time() * 8.0f) + 1.0f) * 0.5f;
				deliveries[i]->SetScale(_scale + _scale * 0.15f * osc);
			}
			else
			{
				deliveries[i]->SetTexture(pizzaIconGray);
				deliveries[i]->SetScale(_scale);
			}

			deliveries[i]->SetPos(pos);
			deliveries[i]->Draw();

			pos.x += deliveries[i]->GetWidth() * _scale + 5.0f;
		}
	}
}

void PQDeliveryStatus::normalizedPosition(const vec2f &setPosition)
{
	_normPos = setPosition;
}

vec2f PQDeliveryStatus::normalizedPosition() const
{
	return _normPos;
}

void PQDeliveryStatus::deliveryCount(uint32_t setDeliveryCount)
{
	while(deliveries.size() < setDeliveryCount)
	{
		deliveries.push_back(AddChild(make_shared<Sprite>()));
		deliveries.back()->category = 0;
		deliveries.back()->SetScale(_scale);
	}

	while(deliveries.size() > setDeliveryCount)
	{
		RemoveChild(deliveries.back());
		deliveries.pop_back();
	}

	_completed = min(_completed, setDeliveryCount);
}

uint32_t PQDeliveryStatus::deliveryCount() const
{
	return deliveries.size();
}

uint32_t PQDeliveryStatus::deliveriesCompleted() const
{
	return _completed;
}

vec2f PQDeliveryStatus::deliveryPos(uint32_t index)
{
	auto game = state()->as<PQGame>();
	 
	vec2f ret = game->guiCamera->NormalizedToWorld(_normPos);
	ret.x += index * pizzaIconFull->width() * _scale + 5.0f;

	return ret;
}

void PQDeliveryStatus::AddReadyPizza(const vec2f &pickedUpAtScreenPos)
{
	RunCoroutine([this, pickedUpAtScreenPos](yield_token<float> yield)
	{
		auto game = state()->as<PQGame>();
		
		vec2f startPos = game->guiCamera->ScreenToWorld(pickedUpAtScreenPos);
		vec2f endPos = deliveryPos(_completed);

		shared_ptr<Sprite> pizza = AddChild(make_shared<Sprite>(pizzaIconFull));
		pizza->category = 2;
		pizza->layer = DrawLayer::UserInterface;
		pizza->SetPos(startPos);
		pizza->SetScale(_scale);
		
		float length = 0.3f;
		float start = Time::time();
		float finish = start + length;

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);
			float t = crv::smoothstep_in(elapsed);
			pizza->SetPos(math::lerp(startPos, endPos, elapsed));

			yield(0);
		}

		RemoveChild(pizza);
	});
}

void PQDeliveryStatus::AddCompletedDelivery(const vec2f &deliveredAtScreenPos)
{
	RunCoroutine([this, deliveredAtScreenPos](yield_token<float> yield)
	{
		auto game = state()->as<PQGame>();
		
		vec2f startPos = game->guiCamera->ScreenToWorld(deliveredAtScreenPos);
		vec2f endPos = deliveryPos(_completed);

		shared_ptr<Sprite> pizza = AddChild(make_shared<Sprite>(pizzaIconChecked));
		pizza->category = 2;
		pizza->layer = DrawLayer::UserInterface;
		pizza->SetPos(startPos);
		pizza->SetScale(_scale);
		
		float length = 0.3f;
		float start = Time::time();
		float finish = start + length;

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);
			float t = crv::smoothstep_in(elapsed);
			pizza->SetPos(math::lerp(startPos, endPos, elapsed));

			yield(0);
		}

		RemoveChild(pizza);

		++_completed;
	});
}
