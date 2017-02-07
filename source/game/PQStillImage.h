/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <assert.h>
#include "State.h"
#include "Time.h"
#include "Sprite.h"
#include "Sound.h"
#include "Stream.h"
#include <string>
#include "Engine.h"
#include <functional>
using namespace std;

struct StillImageDesc
{
	StillImageDesc()
		: fadeInLength(0),
		  sustainLength(0),
		  fadeOutLength(0),
		  tapOut(false){}

	StillImageDesc(const StillImageDesc &other)
		: fnImage(other.fnImage),
		  fnSound(other.fnSound),
		  fadeInLength(other.fadeInLength),
		  sustainLength(other.sustainLength),
		  fadeOutLength(other.fadeOutLength),
		  tapOut(other.tapOut),
		  callback(other.callback){}

	StillImageDesc& operator=(const StillImageDesc &other)
	{
		fnImage = other.fnImage;
		fnSound = other.fnSound;
		fadeInLength = other.fadeInLength;
		sustainLength = other.sustainLength;
		fadeOutLength = other.fadeOutLength;
		tapOut = other.tapOut;
		callback = other.callback;
		return *this;
	}

	string fnImage;
	string fnSound;
	float fadeInLength;
	float sustainLength;
	float fadeOutLength;
	bool tapOut;
	function<void()> callback;
};

class PQStillImage : public State
{
public:
	PQStillImage(const StillImageDesc &desc);
	virtual ~PQStillImage();

	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void OnTouchUp(float x, float y, int id) override;
	virtual void OnBackPressed() override;

private:
	
	StillImageDesc desc;
	shared_ptr<Camera> camera;
	shared_ptr<Shader> shader;
	shared_ptr<Sprite> image;
	shared_ptr<Sound> sound;
	bool done;
};
