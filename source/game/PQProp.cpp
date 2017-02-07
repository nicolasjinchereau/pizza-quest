/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQProp.h"

PQProp::PQProp()
{
	
}

PQProp::~PQProp()
{

}

void PQProp::Start()
{
	PQMapImage::Start();

	body->self_mask(ContactMask::Prop);
	body->others_mask(ContactMask::Player | ContactMask::Vehicle);

	img->layer = DrawLayer::Props;
	img->SetPos(position.x, position.y);
	img->SetAngle(angle);
	img->SetScale(scale);
	img->SetStatic(true);
}
