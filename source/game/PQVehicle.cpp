/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQVehicle.h"
#include "PQGame.h"
#include <assert.h>
#include "PQPlayer.h"
#include "Audio.h"

PQVehicle::PQVehicle()
{
	
}

PQVehicle::~PQVehicle()
{
	
}

void PQVehicle::Freeze()
{
	body->type(RigidBody::Type::Static);
	input.set(0, 0);
	inUse = false;
	hasBoost = false;
	boostLength = 10.0f;
}

void PQVehicle::Unfreeze()
{
	body->type(RigidBody::Type::Dynamic);
	inUse = true;
}

void PQVehicle::DoBoost()
{
	auto game = state()->as<PQGame>();

	if(hasBoost)
		CancelTask(boostRoutine);
	
	boostRoutine = RunCoroutine([=](yield_token<float> yield)
	{
		hasBoost = true;

		game->player->nitrous->Play();

		while(game->nitroGauge->psi() < 1.0f)
		{
			float psi = game->nitroGauge->psi() + Time::deltaTime() * 3.0f;
			psi = math::clamp(psi, 0.0f, 1.0f);
			game->nitroGauge->psi(psi);

			yield(0);
		}

		game->nitroGauge->psi(1.0f);

		float dt = 1.0f / boostLength;

		while(game->nitroGauge->psi() > 0.0f)
		{
			float psi = game->nitroGauge->psi() - Time::deltaTime() * dt;
			psi = math::clamp(psi, 0.0f, 1.0f);
			game->nitroGauge->psi(psi);

			yield(0);
		}

		hasBoost = false;

		game->nitroGauge->psi(0.0f);
	});
}

void PQVehicle::KillBoost()
{
	auto game = state()->as<PQGame>();

	if(hasBoost)
	{
		CancelTask(boostRoutine);
		game->nitroGauge->psi(0.0f);
		game->player->nitrous->Stop();
		hasBoost = false;
	}
}

bool PQVehicle::InUse()
{
	return inUse;
}

void PQVehicle::Start()
{
	img->layer = DrawLayer::Cars + 100;
	
	inUse = false;
	carAngle = 0;
	nAngles = (float)img->GetNumCols();
	degreesPerCol = 360.0f / (float)nAngles;
	angleTolerance = degreesPerCol / 2.0f;
	
	body = make_shared<RigidBody>(state()->physics,
								  Physics::toMeters(position),
								  math::rad(angle - 90.0f),
								  RigidBody::Type::Dynamic);
	
	body->self_mask(ContactMask::Vehicle);
	body->others_mask(ContactMask::Player
					| ContactMask::Vehicle
					| ContactMask::Car
					| ContactMask::Pedestrian
					| ContactMask::MapBorder
					| ContactMask::Prop
					| ContactMask::Structure
					| ContactMask::Tile
					| ContactMask::PowerUp);

	float w = Physics::toMeters(img->GetHeight());
	float h = Physics::toMeters(img->GetWidth());

	float density = 1.0f / (w * h);
	body->AddBox(w * 0.5f, h * 0.5f, vec2f::zero, 0, scale, density);
	body->linearDampening(0.8f);
	body->angularDampening(8.0f);
	
	AddChild(body);

	maxFrontSpeed = 18.0f;
	accelleration = 100.0f;
	sideFriction = 10.0f;
	turnTorque = img->GetWidth() / 72.0f * 200.0f;
	hasBoost = false;
	boostFinish = 0;

	Freeze();
}

void PQVehicle::Update()
{
	auto& player = state()->as<PQGame>()->player;

	if(inUse && player->_carStarted)
	{
		float maxSpeed = hasBoost ? 1.8f * maxFrontSpeed : maxFrontSpeed;

		vec2f velocity = body->velocity();
		vec2f inputForce = input * accelleration;
		vec2f velocityOnInput = velocity.SignedProject(inputForce);
		
		float speedFactor = math::clamp(velocityOnInput.Length() / maxSpeed, 0.0f, 1.0f);
		speedFactor = 1.0f - math::pow<4>(speedFactor);

		inputForce *= speedFactor;

		body->AddForce(inputForce.Project(body->up()), body->position());
		body->AddForce(-velocity.Project(body->right()) * sideFriction, body->position());
		body->AddTorque(turnTorque * speedFactor * input.Dot(body->right()));
	}
}

void PQVehicle::LateUpdate()
{
	if(inUse)
	{
		position = body->pixelPosition();
		img->SetPos(position);
		img->SetAngle(math::deg(body->angle()) + 90.0f);

		state()->as<PQGame>()->mainCamera->LookAt(position.x, position.y);
	}
}

void PQVehicle::OnCollisionEnter(Contact contact)
{
	auto p = contact.body->parent();

 	if(p->tag() == "Car")
	{
		auto car = contact.body->parent()->as<PQCopCar>();
		if(!car->destroyed())
		{
			car->destroyed() = true;

			KillBoost();

			auto game = state()->as<PQGame>();
			game->DoExplosion(contact.point);
			game->AddStrike(PQGame::StrikeReason::HitCar);
		}
	}
	else if(contact.body->parent()->tag() == "Pedestrian")
	{
		if(body->velocity().Length() >= 1.0f)
		{
			KillBoost();

			contact.body->parent()->as<PQPedestrian>()->SetSplattered();

			auto game = state()->as<PQGame>();
			game->AddStrike(PQGame::StrikeReason::HitPerson);
		}
	}
}
