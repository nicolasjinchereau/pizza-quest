/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Button.h"
#include "Engine.h"

Button::Button()
{
	down = false;
	buttonClicked = 0;
	pID = -1;	
}

bool Button::Open(const char *filename)
{
	_image = AddChild(make_shared<Sprite>());
	
	if(!_image->Open(filename))
		return false;

	_image->layer = DrawLayer::UserInterface;
	_image->category = 2;
	_image->SetNumRows(2);

	return true;
}

void Button::SetPosition(float xn, float yn)
{
	normalizedPosition.set(xn, yn);
}

vec2f Button::position() const
{
	return normalizedPosition;
}

Button::~Button()
{

}

bool Button::TryPress(float wx, float wy, int id)
{
	if( _image->PointInside(wx, wy) )
	{
		if(!down)
		{
			down = true;
			pID = id;
			_image->SetRow(1);
			if(onButtonDown) onButtonDown();
		}
	}
	
	return down;
}

bool Button::TryRelease(int id)
{
	if(down && id == pID)
	{
		down = false;
		pID = -1;
		_image->SetRow(0);
		
		if(buttonClicked == 0)
		{
			buttonClicked = Time::time();
		}

		return true;
	}

	return false;
}

void Button::ForceRelease()
{
	if(down)
	{
		down = false;
		pID = -1;
		_image->SetRow(0);
	}
}

bool Button::IsDown()
{
	return down;
}

void Button::SetPressEvent(const function<void()> &fn)
{
	onButtonDown = fn;
}

void Button::SetReleaseEvent(const function<void()> &fn)
{
	onButtonUp = fn;
}

void Button::Update()
{
	// button was clicked
	if(buttonClicked > 0)
	{
		// dont run the up function until 50ms after the click
		if(Time::time() - buttonClicked > 0.05f)
		{
			buttonClicked = 0;
			if(onButtonUp) onButtonUp();
		}
	}
}

void Button::PreDrawUpdate()
{
	_image->SetPos( Camera::activeCamera()->NormalizedToWorld(normalizedPosition) );
}

shared_ptr<Sprite> Button::image()
{
	return _image;
}

void Button::image(const shared_ptr<Sprite> &setImage)
{
	this->_image = setImage;
}
