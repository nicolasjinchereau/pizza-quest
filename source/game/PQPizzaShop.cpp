/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "State.h"
#include "PQPizzaShop.h"
#include "PQPlayer.h"
#include "PQPlayer.h"
#include "PQGame.h"
#include <memory>

PQPizzaShop::PQPizzaShop()
{
	
}

PQPizzaShop::~PQPizzaShop()
{

}

void PQPizzaShop::Start()
{
	PQMapImage::Start();

	img->layer = DrawLayer::Structures;
	img->SetStatic(true);

	body = make_shared<RigidBody>(state()->physics,
								  GetResource()->collision_shapes,
								  position,
								  RADIANS(angle),
								  scale);

	AddChild(body);
}
