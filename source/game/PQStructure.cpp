/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQStructure.h"

PQStructure::PQStructure()
{
	
}

void PQStructure::Start()
{
	PQMapImage::Start();
	
	img->layer = DrawLayer::Structures;
	img->SetStatic(true);

	body->self_mask(ContactMask::Structure);
	body->others_mask(ContactMask::Player | ContactMask::Vehicle);
}

PQStructure::~PQStructure()
{

}
