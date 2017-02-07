/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQGameLoader.h"
#include "PQMenu.h"
#include "Time.h"
#include "PQGame.h"
#include "State.h"
#include "Audio.h"

PQGameLoader::PQGameLoader()
{
	
}

void PQGameLoader::Start()
{
	PizzaQuest::sounds().themeSong.Stop();

	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);

	background = AddChild(make_shared<Sprite>());
	background->Open("assets\\Images\\Menus\\BackgroundEmpty.png");
	background->SetPos(camera->NormalizedToWorldX(0.5f), camera->NormalizedToWorldY(0.5f) );

	progressBar = AddChild(make_shared<PQProgressBar>());
	progressBar->OpenImages("assets/Images/GUI/progressBackground.png",
						   "assets/Images/GUI/progressBorder.png",
						   "assets/Images/GUI/progressFill.png");

	progressBar->SetPosition(camera->NormalizedToWorldX(0.5f),
							 camera->NormalizedToWorldY(0.85f));

	progressBar->SetProgress(0.0f);

	game = make_shared<PQGame>();

	loader = coroutine<float>(&PQGame::Initialize, game);
}

PQGameLoader::~PQGameLoader()
{

}

void PQGameLoader::Update()
{
	if(loader())
		progressBar->SetProgress(loader.get());
	else
		Engine::SetState(game);
}

//void PQGameLoader::Draw()
//{
//	Camera::activeCamera(camera);
//	background->Draw();
//	progressBar->Draw();
//}
