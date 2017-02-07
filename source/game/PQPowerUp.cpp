/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPowerUp.h"
#include "State.h"
#include "PQGame.h"
#include "PQPlayer.h"
#include "Audio.h"
#include "State.h"

PQPowerUp::PQPowerUp(Type type)
	: _type(type), nextBlink(0)
{
	
}

PQPowerUp::~PQPowerUp()
{

}

void PQPowerUp::Start()
{
	img->layer = DrawLayer::Tiles + 700;
	img->SetPos(position.x, position.y);
	img->SetScale(scale);
	img->SetRow(0);
	img->SetStatic(true);

	body = make_shared<RigidBody>(state()->physics,
								  Physics::toMeters(position),
								  angle,
								  RigidBody::Type::Trigger);

	ContactMask others;
	
	if(_type == Type::Speed)
		others = ContactMask::Vehicle;
	else
		others = ContactMask::Vehicle | ContactMask::Player;

	body->self_mask(ContactMask::PowerUp);
	body->others_mask(others);

	body->AddCircle(vec2f::zero, Physics::toMeters(img->GetWidth() * 0.5f), scale, 0, 0, 0);

	AddChild(body);
}

void PQPowerUp::Update()
{
	if(Time::time() > nextBlink)
	{
		nextBlink = Time::time() + 0.5f;
		img->SetRow(img->GetRow() == 0 ? 1 : 0);
	}
}

void PQPowerUp::OnCollisionEnter(Contact contact)
{
	auto game = static_pointer_cast<PQGame>(state());

	switch(_type)
	{
	case Type::Health:
		game->player->health() += 20;
		break;

	case Type::Speed:
		game->player->car->DoBoost();
		break;

	case Type::Time:
		game->timer->AddTime(30);
		break;
	}

	auto st = state();

	st->RunAfterUpdate([st, this]{
		PizzaQuest::sounds().whip->Play();
		st->RemoveChild(this);
	});
}
