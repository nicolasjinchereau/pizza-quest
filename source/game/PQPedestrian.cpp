/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPedestrian.h"
#include "PQGame.h"
#include "Engine.h"
#include "Audio.h"
#include "PizzaQuest.h"

PQPedestrian::PQPedestrian(PQGame *game)
	: PQPathFinder(&game->pedGraph, &game->pedGoals, 60.0f)
{
	
}

PQPedestrian::~PQPedestrian()
{

}

void PQPedestrian::Start()
{
	PQPathFinder::Start();

	img->category = 0;

	category = 1;
	layer = DrawLayer::Characters;
	
	float degreesPerRow = 360.0f / (float)nAngles;
	angleTolerance = degreesPerRow / 2.0f;
	
	float ang = 0.0f;

	for (int y = 0; y < nAngles; y++)
	{
		animations[y] = AddChild(make_shared<Animation>());
		animations[y]->sprite(img);

		for (int x = 1; x < 17; x++)
		{
			animations[y]->AddFrame(y, x);
		}

		angles[y] = ang;
		ang += degreesPerRow;
	}

	animation = nullptr;
	isSplattered = false;

	splatter = AddChild(make_shared<Sprite>(PizzaQuest::textures().splatter));
	splatter->SetVisible(true);
	splatter->SetScale(0.5f);
	splatter->category = 0;

	body = make_shared<RigidBody>(state()->physics,
					              Physics::toMeters(position),
					              0.0f,
					              RigidBody::Type::Trigger);
	
	body->self_mask(ContactMask::Pedestrian);
	body->others_mask(ContactMask::Vehicle);

	body->AddCircle(vec2f::zero, Physics::toMeters(10.0f), 1.0f, 0.1f, 0.02f, 0.0f);

	AddChild(body);

	feelerShape.SetAsBox(1, 0.5f, b2Vec2(1, 0), 0);

	//carShape.m_radius = 4.0f;
}

void PQPedestrian::LateUpdate()
{
	PQPathFinder::LateUpdate();

	const vec2f &dir = PQPathFinder::GetDirection();

	spriteRow = -1;

	if(dir.x != 0.0f
	|| dir.y != 0.0f)
	{
		float angle = atan2(dir.y, dir.x);

		b2Transform xfFeeler(body->b2_Body()->GetPosition(), b2Rot(angle));
		
		bool touchingCar = false;

		auto game = state()->as<PQGame>();

		for(auto it = game->cars.begin(), itEnd = game->cars.end();
			!touchingCar && it != itEnd;
			++it)
		{
			auto carBody = (*it)->body->b2_Body();

			// cars should only have one shape(a rectangle)
			auto carShape = carBody->GetFixtureList()->GetShape();

			if(b2TestOverlap(&feelerShape, 0,
							 carShape, 0,
							 xfFeeler,
							 carBody->GetTransform()))
			{
				touchingCar = true;
			}
		}

		tween->paused(touchingCar);

		float degAngle = math::deg(angle);

		if(degAngle < -angleTolerance)
			degAngle += 360.0f;

		for(int i = 0; i < nAngles; i++)
		{
			if(degAngle >= angles[i] - angleTolerance
			&& degAngle <= angles[i] + angleTolerance)
			{
				spriteRow = i;
				break;
			}
		}
	}

	if(spriteRow >= 0)
	{
		if(animation != animations[spriteRow].get())
		{
			if(animation) animation->Stop();
			animation = animations[spriteRow].get();
			animation->Play(true, false);
		}
	}
	else
	{
		if(animation)
		{
			animation->Stop();
			animation = nullptr;
		}

		col = 0;
	}

	body->pixelPosition(position);
}

void PQPedestrian::Draw()
{
	if(!isSplattered)
	{
		if(animation == nullptr)
		{
			PQMapImage::Draw();
		}
		else
		{
			img->SetAngle(angle);
			img->SetScale(scale);
			img->SetPos(position.x, position.y);
			animation->Draw();
		}
	}
	else
	{
		splatter->Draw();
	}
}

void PQPedestrian::OnCollisionEnter(Contact contact)
{
	
}

void PQPedestrian::SetSplattered()
{
	PQPathFinder::LeavePath();

	isSplattered = true;
	splatter->SetPos(position);
	splatter->SetAngle((float)rand() / (float)RAND_MAX * 360.0f);
	
	layer = DrawLayer::Tiles + 500;

	PizzaQuest::sounds().scream->Play();

	RunAfterUpdate([&](){ body->active(false); });
}
