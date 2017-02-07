/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQTile.h"
#include "State.h"

PQTile::PQTile()
{
	
}

PQTile::~PQTile()
{

}

void PQTile::Start()
{
	img->layer = DrawLayer::Tiles;
	img->SetStatic(true);
	
	body = make_shared<RigidBody>(state()->physics,
								  GetResource()->collision_shapes,
								  position,
								  RADIANS(angle),
								  scale);
	body->friction(0);
	body->self_mask(ContactMask::Tile);
	body->others_mask(ContactMask::Vehicle);

	AddChild(body);
}
