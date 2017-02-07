/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQJoystick.h"
#include "PQGame.h"

PQJoystick::PQJoystick(float wx, float wy)
{
	position.set(wx, wy);

	length = 0.0f;
	radius = 55.0f;
	active = false;
	pID = -1;
}

PQJoystick::~PQJoystick()
{

}

void PQJoystick::GetLoadTasks(vector<function<void()>> &loadTasks)
{
	loadTasks.emplace_back([this]{
		ring = AddChild(make_shared<Sprite>());
		ring->Open("assets\\Images\\GUI\\ThumbRing.png");
		ring->SetScale(1.0f);
		ring->SetPos(position);
		ring->category = 2;
		ring->layer = DrawLayer::UserInterface;
		ring->SetVisible(false);
	});

	loadTasks.emplace_back([this]{
		center = AddChild(make_shared<Sprite>());
		center->Open("assets\\Images\\GUI\\ThumbCenter.png");
		center->SetScale(1.0f);
		center->SetPos(position);
		center->category = 2;
		center->layer = DrawLayer::UserInterface;
		center->SetVisible(false);
	});
}


void PQJoystick::Start()
{
	
}

void PQJoystick::Press(float wx, float wy, int id)
{
	if(!active)
	{
		position.set(wx, wy);
		ring->SetPos(position);
		center->SetPos(position);
	
		ring->SetVisible(true);
		center->SetVisible(true);
		active = true;
		pID = id;
	}
}

void PQJoystick::SetThumbPos(float wx, float wy, int id)
{
	if(active && id == pID)
	{
		thumb.set(wx - position.x, wy - position.y);
		length = thumb.Length();

		// resize thumb vector if it's too far off center
		if(length > radius)
		{
			thumb /= length;
			thumb *= radius;
		}

		center->SetPos(position + thumb);
	}
}
	
void PQJoystick::TryRelease(int id)
{
	if(active && id == pID)
	{
		ring->SetVisible(false);
		center->SetVisible(false);
		active = false;
		pID = -1;
	}
}

void PQJoystick::ForceRelease()
{
	if(active)
	{
		ring->SetVisible(false);
		center->SetVisible(false);
		active = false;
		pID = -1;
	}
}

const vec2f &PQJoystick::GetThumbVec()
{
	if(active)
		clampedThumb = thumb / radius;
	else
		clampedThumb.set(0, 0);

	return clampedThumb;
}
