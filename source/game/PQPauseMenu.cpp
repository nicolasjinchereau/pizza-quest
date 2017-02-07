/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPauseMenu.h"
#include "PQMenu.h"
#include "PQGameLoader.h"
#include "Audio.h"
#include "PQGame.h"

PQPauseMenu::PQPauseMenu(shared_ptr<PQGame> game)
	: game(game)
{
	oldSongGain = game->mainSong->GetGain();
	game->mainSong->SetGain(0.05f);
}

PQPauseMenu::~PQPauseMenu()
{
	game->mainSong->SetGain(oldSongGain);
}

void PQPauseMenu::Start()
{
	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);

	background = AddChild(make_shared<Sprite>());
	background->Open("assets\\Images\\Menus\\Background.png");
	background->SetPos(camera->NormalizedToWorldX(0.5f),
					  camera->NormalizedToWorldY(0.5f) );

	btnResume = AddChild(make_shared<Button>());
	btnRetry = AddChild(make_shared<Button>());
	btnQuit = AddChild(make_shared<Button>());

	btnResume->Open("assets\\Images\\Menus\\btnResume.png");
	btnRetry->Open("assets\\Images\\Menus\\btnRetry.png");
	btnQuit->Open("assets\\Images\\Menus\\btnQuit.png");
	
	btnResume->SetPosition(0.26f, 0.189583f);
	btnRetry->SetPosition(0.26f, 0.45f);
	btnQuit->SetPosition(0.26f, 0.710417f);
	
	btnResume->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
	btnQuit->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
	btnRetry->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
	
	btnResume->SetReleaseEvent( [this]{ Resume(); } );
	btnQuit->SetReleaseEvent( [this]{ Quit(); } );
	btnRetry->SetReleaseEvent( [this]{ Retry(); } );
}

void PQPauseMenu::Quit()
{
	Engine::SetState( make_shared<PQMenu>() );
}

void PQPauseMenu::Retry()
{
	Engine::SetState( make_shared<PQGameLoader>() );
}

void PQPauseMenu::Resume()
{
	Engine::PopState();
}

//void PQPauseMenu::Draw()
//{
// 	background->Draw();
//	btnResume->Draw();
//	btnRetry->Draw();
//	btnQuit->Draw();
//}

void PQPauseMenu::OnTouchDown(float x, float y, int id)
{
	const float wx = camera->ScreenToWorldX(x);
	const float wy = camera->ScreenToWorldX(y);

	btnResume->TryPress(wx, wy, id);
	btnQuit->TryPress(wx, wy, id);
	btnRetry->TryPress(wx, wy, id);
}

void PQPauseMenu::OnTouchUp(float x, float y, int id)
{
	btnResume->TryRelease(id);
	btnQuit->TryRelease(id);
	btnRetry->TryRelease(id);
}
