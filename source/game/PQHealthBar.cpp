/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQHealthBar.h"
#include "Engine.h"
#include "State.h"
#include "PQGame.h"
#include "Camera.h"
#include "curves.h"
#include <cmath>

PQHealthBar::PQHealthBar()
{
	category = 2;
	layer = DrawLayer::UserInterface;

	_normPos = vec2f::zero;
	_visible = true;
	_scale = 0.7f;
	_fill = 0;
	_smoothFill = 0;
	_smoothSpeed = 0.5f;
}

PQHealthBar::~PQHealthBar()
{

}

void PQHealthBar::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		tintShader = AddChild(make_shared<Shader>());
		tintShader->Load("assets\\Shaders\\default.vert",
						 "assets\\Shaders\\tinted.frag");
	});

	loadTasks.emplace_back([this]{
		colorizeShader = AddChild(make_shared<Shader>());
		colorizeShader->Load("assets\\Shaders\\default.vert",
							 "assets\\Shaders\\colorize.frag");
	});

	loadTasks.emplace_back([this]{
		heart = AddChild(make_shared<Sprite>());
		heart->Open("assets\\Images\\GUI\\Heart.png");
		heart->SetScale(_scale);
		heart->category = 0;
	});

	loadTasks.emplace_back([this]{
		background = AddChild(make_shared<Sprite>());
		background->Open("assets\\Images\\GUI\\HealthBarBackground.png");
		background->SetScale(_scale);
		background->category = 0;
	});

	loadTasks.emplace_back([this]{
		border = AddChild(make_shared<Sprite>());
		border->Open("assets\\Images\\GUI\\HealthBarBorder.png");
		border->SetScale(_scale);
		border->category = 0;
	});

	loadTasks.emplace_back([this]{
		filler = AddChild(make_shared<Sprite>());
		filler->Open("assets\\Images\\GUI\\HealthBarFill.png");
		filler->SetScale(_scale);
		filler->category = 0;
	});
}

void PQHealthBar::Start()
{
	
}

void PQHealthBar::LateUpdate()
{
	if(_fill > _smoothFill)
		_smoothFill = min(_smoothFill + Time::deltaTime() * _smoothSpeed, _fill);
	else if(_fill < _smoothFill)
		_smoothFill = max(_smoothFill - Time::deltaTime() * _smoothSpeed, _fill);
}

void PQHealthBar::Draw()
{
	if(_visible)
	{
		auto game = state()->as<PQGame>();
		
		heart->SetPos(game->guiCamera->NormalizedToWorld(_normPos - vec2f(0.26f * _scale, 0)));
		heart->Draw();

		background->SetPos(game->guiCamera->NormalizedToWorld(_normPos));
		background->Draw();

		filler->clipBorder(Rect(0, 0, (1.0f - _smoothFill) * filler->GetWidth(), 0));
		filler->SetPos(game->guiCamera->NormalizedToWorld(_normPos));
		filler->Draw();

		border->SetPos(game->guiCamera->NormalizedToWorld(_normPos));
		border->Draw();
	}
}

void PQHealthBar::DoRedPulse()
{
	if(auto sp = pulseRoutine.lock())
		CancelTask(sp);

	pulseRoutine = RunCoroutine([&](yield_token<float> yield)
	{
		heart->SetShader(colorizeShader);
		border->SetShader(colorizeShader);
		filler->SetShader(colorizeShader);

		colorizeShader->SetActive();
		colorizeShader->SetUniform("uTint", Color::red);
		
		int idTintStrength = colorizeShader->GetUniformID("uTintStrength");

		float length = 2.0f;
		float start = Time::time();
		float finish = start + length;

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);

			float heartGrow = abs(sin(elapsed * 1.5f * math::rad(360)));

			float barGrow = (elapsed <= 0.1f) ? (elapsed / 0.1f) : 1.0f - math::clamp(elapsed - 0.1f, 0.0f, 0.5f) / 0.5f;
			float c = crv::arc_oct(elapsed);
			
			float heartScale = _scale + _scale * 0.4f * heartGrow;
			float barScale = _scale + _scale * 0.03f * barGrow;

			heart->SetScale(heartScale);
			background->SetScale(barScale);
			filler->SetScale(barScale);
			border->SetScale(barScale);

			colorizeShader->SetActive();
			colorizeShader->SetUniform(idTintStrength, c);

			yield(0);
		}

		heart->SetShader(Graphics::defaultShader());
		border->SetShader(Graphics::defaultShader());
		filler->SetShader(Graphics::defaultShader());

		heart->SetScale(_scale);
		background->SetScale(_scale);
		filler->SetScale(_scale);
		border->SetScale(_scale);
	});
}

void PQHealthBar::DoBlackPulse()
{
	if(auto sp = pulseRoutine.lock())
		CancelTask(sp);

	pulseRoutine = RunCoroutine([&](yield_token<float> yield)
	{
		heart->SetShader(tintShader);
		border->SetShader(tintShader);
		filler->SetShader(tintShader);

		int id = tintShader->GetUniformID("uTint");
		
		float length = 2.0f;
		float start = Time::time();
		float finish = start + length;

		while(Time::time() <= finish)
		{
			float elapsed = math::clamp((Time::time() - start) / length, 0.0f, 1.0f);

			float heartGrow = abs(sin(elapsed * 1.5f * math::rad(360)));

			float barGrow = (elapsed <= 0.1f) ? (elapsed / 0.1f) : 1.0f - math::clamp(elapsed - 0.1f, 0.0f, 0.5f) / 0.5f;
			float c = crv::inv_arc_quad(elapsed);
			
			float heartScale = _scale + _scale * 0.4f * heartGrow;
			float barScale = _scale + _scale * 0.03f * barGrow;

			heart->SetScale(heartScale);
			background->SetScale(barScale);
			filler->SetScale(barScale);
			border->SetScale(barScale);

			tintShader->SetActive();
			tintShader->SetUniform(id, Color(c, c, c));

			yield(0);
		}

		heart->SetShader(Graphics::defaultShader());
		border->SetShader(Graphics::defaultShader());
		filler->SetShader(Graphics::defaultShader());

		heart->SetScale(_scale);
		background->SetScale(_scale);
		filler->SetScale(_scale);
		border->SetScale(_scale);
	});
}

void PQHealthBar::normalizedPosition(const vec2f &setPosition)
{
	_normPos = setPosition;
}

vec2f PQHealthBar::normalizedPosition() const
{
	return _normPos;
}
