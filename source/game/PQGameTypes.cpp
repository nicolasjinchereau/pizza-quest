/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQGameTypes.h"
#include "State.h"

/*****************************
PQ RESOURCE
*****************************/

PQResource::PQResource()
{
	type = RES_BASE;
}

/*****************************
PQ RESOURCE IMAGE
*****************************/

PQResImage::PQResImage()
{
	type = RES_IMAGE;
}

PQResImage::~PQResImage()
{
	
}

void PQResImage::Init()
{
	tex = make_shared<Texture>();
	tex->Open(source_file);
}

/*****************************
PQ RESOURCE SOUND
*****************************/

PQResSound::PQResSound()
{
	
}

void PQResSound::Init()
{
	snd = AddChild(make_shared<Sound>());
	snd->Open(source_file);
}

/*****************************
PQ MAP OBJECT
*****************************/
PQMapObject::PQMapObject()
{
	type = MAP_BASE;
}

PQMapObject::~PQMapObject()
{

}

/*****************************
PQ MAP IMAGE
*****************************/

PQMapImage::PQMapImage()
{
	body = NULL;
	type = MAP_IMAGE;
	resource = NULL;
	position = vec2f::zero;
	row = 0;
	col = 0;
	angle = 0;
	scale = 1.0f;
	value1 = 0;
	value2 = 0;
}

PQMapImage::~PQMapImage()
{
	
}

shared_ptr<PQResImage> PQMapImage::GetResource()
{
	return static_pointer_cast<PQResImage>(resource);
}

void PQMapImage::Start()
{
	body = make_shared<RigidBody>(state()->physics,
								  GetResource()->collision_shapes,
								  position,
								  RADIANS(angle),
								  scale);

	AddChild(body);
}
