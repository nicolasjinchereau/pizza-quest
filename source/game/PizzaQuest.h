/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Path.h"
#include "PQStillImage.h"
#include "PQMenu.h"
#include "PlayerProfile.h"
#include "includes.h"
#include "WindowsApp.h"
#include "Trace.h"
#include "Sound.h"

class SharedSounds
{
public:
	Stream themeSong;
	shared_ptr<Sound> button;
	shared_ptr<Sound> explosion;
	shared_ptr<Sound> scream;
	shared_ptr<Sound> whip;
	shared_ptr<Sound> error;

	SharedSounds() {
		themeSong.Open("assets\\Sounds\\Music\\ThemeSong.mp3");
		themeSong.SetLoop(true);
		
		button = make_shared<Sound>("assets\\Sounds\\Effects\\btn01.wav");
		explosion = make_shared<Sound>("assets\\Sounds\\Effects\\explosion.wav");
		scream = make_shared<Sound>("assets\\Sounds\\Effects\\scream01.wav");
		whip = make_shared<Sound>("assets\\Sounds\\Effects\\whip.wav");
		error = make_shared<Sound>("assets\\Sounds\\Effects\\error.wav");
	}
};

class SharedTextures
{
public:
	shared_ptr<Texture> explosion;
	shared_ptr<Texture> splatter;
	shared_ptr<Texture> burnCopCar;

	SharedTextures()
	{
		explosion = make_shared<Texture>("assets\\Images\\Extra\\explosion.png");
		splatter = make_shared<Texture>("assets\\Images\\Extra\\splatter.png");
		burnCopCar = make_shared<Texture>("assets\\Images\\Characters\\copCarBurnt.png");
	}
};

class PizzaQuest : public WindowsApp
{
	static shared_ptr<SharedSounds> _sounds;
	static shared_ptr<SharedTextures> _textures;
public:
	static SharedSounds& sounds();
	static SharedTextures& textures();

	PizzaQuest(int width = 1280, int height = 800)
		: WindowsApp("Pizza Quest", width, height){}

	virtual void OnInitialize() override;
	virtual void OnTerminate() override;
};
