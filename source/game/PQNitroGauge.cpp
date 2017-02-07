/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQNitroGauge.h"
#include "State.h"
#include "PQGame.h"

PQNitroGauge::PQNitroGauge()
{
	_psi = 0;
	_normPos = vec2f::zero;
	_minAngle = 3;
	_maxAngle = 270;
	_visible = true;

	category = 2;
	layer = DrawLayer::UserInterface;
}

PQNitroGauge::~PQNitroGauge()
{

}

void PQNitroGauge::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	float scale = 0.4f;

	loadTasks.emplace_back([this, scale]{
		gauge = AddChild(make_shared<Sprite>());
		gauge->Open("assets\\Images\\GUI\\NitroGauge.png");
		gauge->SetScale(scale);
		gauge->category = 0;
	});

	loadTasks.emplace_back([this, scale]{
		needle = AddChild(make_shared<Sprite>());
		needle->Open("assets\\Images\\GUI\\NitroGaugeNeedle.png");
		needle->SetScale(scale);
		needle->category = 0;
	});
}

void PQNitroGauge::Start()
{
	
}

void PQNitroGauge::Draw()
{
	if(_visible)
	{
		auto game = state()->as<PQGame>();
	
		gauge->SetPos(game->guiCamera->NormalizedToWorld(_normPos));
		needle->SetPos(game->guiCamera->NormalizedToWorld(_normPos));
		needle->SetAngle(_minAngle + _psi * (_maxAngle - _minAngle));

		gauge->Draw();
		needle->Draw();
	}
}

void PQNitroGauge::normalizedPosition(const vec2f &setPosition)
{
	_normPos = setPosition;
}

vec2f PQNitroGauge::normalizedPosition() const
{
	return _normPos;
}

float PQNitroGauge::psi() const
{
	return _psi;
}

void PQNitroGauge::psi(float setPsi)
{
	_psi = setPsi;
}

bool PQNitroGauge::visible() const
{
	return _visible;
}

void PQNitroGauge::visible(bool setVisible)
{
	_visible = setVisible;
}
