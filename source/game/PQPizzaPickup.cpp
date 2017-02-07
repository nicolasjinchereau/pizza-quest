/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPizzaPickup.h"
#include "PQGame.h"
#include <coroutine.h>
using namespace coroutines;

PQPizzaPickup::PQPizzaPickup()
{
	
}

PQPizzaPickup::~PQPizzaPickup()
{

}

void PQPizzaPickup::Start()
{
	shader = make_shared<Shader>("assets\\Shaders\\default.vert", "assets\\Shaders\\tinted.frag");
	AddChild(shader);

	img->layer = DrawLayer::Tiles + 100;
	img->SetPos(position.x, position.y);
	img->SetScale(scale);
	img->SetShader(shader);
	img->SetStatic(true);

	body = make_shared<RigidBody>(state()->physics,
								  Physics::toMeters(position),
								  angle,
								  RigidBody::Type::Trigger);

	body->self_mask(ContactMask::Pickup);
	body->others_mask(ContactMask::Player);
	body->AddCircle(vec2f::zero, Physics::toMeters(img->GetWidth() * 0.5f), scale, 0, 0, 0);

	AddChild(body);

	body->active(false);
	img->SetVisible(false);
}

void PQPizzaPickup::LateUpdate()
{
	if(img->IsVisible())
	{
		float s = sin(Time::time() * 6);
		float c = ((s + 1.0f) * 0.5f) * 0.7f + 0.3f;
		shader->SetActive();
		shader->SetUniform("uTint", Color(c, c, c));
	}
}

void PQPizzaPickup::OnCollisionEnter(Contact contact)
{
	if(contact.body->parent()->tag() == "Player")
	{
		state()->as<PQGame>()->FinishPickup();
	}
}

void PQPizzaPickup::Enable(bool enable)
{
	body->active(enable);
	img->SetVisible(enable);
}
