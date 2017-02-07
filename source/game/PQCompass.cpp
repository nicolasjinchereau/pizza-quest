/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQCompass.h"
#include "PQGame.h"

PQCompass::PQCompass()
{
	arrow = nullptr;
	scale = 0.7f;

	category = 2;
	layer = DrawLayer::UserInterface;
}

void PQCompass::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		compassBackground = AddChild(make_shared<Sprite>());
		compassBackground->Open("assets\\Images\\GUI\\compass.png");
		compassBackground->SetScale(scale);
		compassBackground->category = 0;
	});

	loadTasks.emplace_back([this]{
		greenArrow = AddChild(make_shared<Sprite>());
		greenArrow->Open("assets\\Images\\GUI\\greenArrow.png");
		greenArrow->SetScale(1.5f * scale);
		greenArrow->category = 0;
	});

	loadTasks.emplace_back([this]{
		redArrow = AddChild(make_shared<Sprite>());
		redArrow->Open("assets\\Images\\GUI\\redArrow.png");
		redArrow->SetScale(1.5f * scale);
		redArrow->category = 0;
	});
}

void PQCompass::Start()
{
	
}

PQCompass::~PQCompass()
{

}

void PQCompass::normalizedPosition(const vec2f &setPosition)
{
	_normalizedPosition = setPosition;
}

vec2f PQCompass::normalizedPosition() const
{
	return _normalizedPosition;
}

void PQCompass::SetDestination(const vec2f &dest)
{
	destination = dest;
}

void PQCompass::ShowGreenArrow()
{
	arrow = greenArrow.get();
}

void PQCompass::ShowRedArrow()
{
	arrow = redArrow.get();
}

void PQCompass::ShowNoArrow()
{
	arrow = nullptr;
}

void PQCompass::Draw()
{
	auto game = Engine::GetState<PQGame>();

	vec2f screenPos = game->guiCamera->NormalizedToWorld(_normalizedPosition);

	compassBackground->SetPos(screenPos);
	compassBackground->Draw();

	if(arrow)
	{
		vec2f playerPos;

		if(game->player->car != NULL)
			playerPos = game->player->car->body->pixelPosition();
		else
			playerPos = game->player->body->pixelPosition();

		vec2f toDest = destination - playerPos;
		toDest.Normalize();
		toDest *= 36 * scale; //29

		float angle = math::deg(atan2(toDest.y, toDest.x));
		arrow->SetPos(screenPos + toDest);
		arrow->SetAngle(angle);
		arrow->Draw();
	}
}
