/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PizzaQuest.h"

shared_ptr<SharedSounds> PizzaQuest::_sounds;
shared_ptr<SharedTextures> PizzaQuest::_textures;

SharedSounds& PizzaQuest::sounds()
{
	return *_sounds.get();
}

SharedTextures& PizzaQuest::textures()
{
	return *_textures.get();
}

void PizzaQuest::OnInitialize()
{
	_sounds = make_shared<SharedSounds>();
	_textures = make_shared<SharedTextures>();

	if(!PlayerProfile::LoadProfile("player.dat"))
	{
		PlayerProfile::AddMap("assets\\Map01.pqm", "assets\\Sounds\\Music\\Warehouse.mp3");
		PlayerProfile::AddMap("assets\\Map02.pqm", "assets\\Sounds\\Music\\Warehouse2.mp3");
		PlayerProfile::AddMap("assets\\Map03.pqm", "assets\\Sounds\\Music\\30SecondsOrYouDie.mp3");
		PlayerProfile::AddMap("assets\\Map04.pqm", "assets\\Sounds\\Music\\LittleItaly.mp3");
		PlayerProfile::AddMap("assets\\Map05.pqm", "assets\\Sounds\\Music\\Prison.mp3");
		PlayerProfile::AddMap("assets\\Map06.pqm", "assets\\Sounds\\Music\\DrivingTechno.mp3");
	}

	StillImageDesc ggamesLogoDesc;
	ggamesLogoDesc.fadeInLength = 0.5f;
	ggamesLogoDesc.sustainLength = 5;
	ggamesLogoDesc.fadeOutLength = 0.5f;
	ggamesLogoDesc.fnImage = "assets/Images/Title Screens/GGamesLogo.png";
	ggamesLogoDesc.tapOut = true;
	ggamesLogoDesc.callback = [this]()
	{
		StillImageDesc pqLogoDesc;
		pqLogoDesc.fadeInLength = 0.5f;
		pqLogoDesc.sustainLength = 3;
		pqLogoDesc.fadeOutLength = 0.5f;
		pqLogoDesc.fnImage = "assets/Images/Title Screens/PQLogo.png";
		pqLogoDesc.tapOut = true;
		pqLogoDesc.callback = [this]
		{
			Engine::SetState( make_shared<PQMenu>() );
		};

		Engine::SetState( make_shared<PQStillImage>(pqLogoDesc) );
	};

	Engine::SetState( make_shared<PQStillImage>(ggamesLogoDesc) );
}

void PizzaQuest::OnTerminate()
{
	_sounds.reset();
	_textures.reset();

	PlayerProfile::SaveProfile("player.dat");
}
