/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQScoreScreen.h"
#include "PizzaQuest.h"
#include "PQMenu.h"
#include "PQGameLoader.h"
#include "Audio.h"

PQScoreScreen::PQScoreScreen()
{

}

PQScoreScreen::~PQScoreScreen()
{
	
}

void PQScoreScreen::Quit()
{
	Engine::SetState( make_shared<PQMenu>() );
}

void PQScoreScreen::Retry()
{
	Engine::SetState( make_shared<PQGameLoader>() );
}

void PQScoreScreen::Next()
{
	Engine::SetState( make_shared<PQGameLoader>() );
}

void PQScoreScreen::Start()
{
	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);
	
	scoreBackground = AddChild(make_shared<Sprite>());
	scoreBackground->Open("assets\\Images\\Menus\\BackgroundEmpty.png");
	scoreBackground->SetPos(camera->NormalizedToWorldX(0.5f),
						    camera->NormalizedToWorldY(0.5f));

	LevelData& level = PlayerProfile::currentLevelData();

	//WIN
	if(level.completed)
	{
		PlayerProfile::AdvanceLevel();

		btnNext = AddChild(make_shared<Button>());
		btnQuit = AddChild(make_shared<Button>());
		
		btnNext->Open("assets\\Images\\Menus\\btnNext.png");
		btnQuit->Open("assets\\Images\\Menus\\btnQuit.png");

		btnNext->SetPosition(0.26f, 0.189583f);
		btnQuit->SetPosition(0.26f, 0.45f);

		btnNext->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
		btnQuit->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
		
		btnNext->SetReleaseEvent( [this]{ Next(); } );
		btnQuit->SetReleaseEvent( [this]{ Quit(); } );
	}
	else // LOSS
	{
		btnRetry = AddChild(make_shared<Button>());
		btnQuit = AddChild(make_shared<Button>());
		
		btnRetry->Open("assets\\Images\\Menus\\btnRetry.png");
		btnQuit->Open("assets\\Images\\Menus\\btnQuit.png");

		btnRetry->SetPosition(0.26f, 0.189583f);
		btnQuit->SetPosition(0.26f, 0.45f);
		
		btnRetry->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
		btnQuit->SetPressEvent( []{ PizzaQuest::sounds().button->Play(); } );
		
		btnRetry->SetReleaseEvent( [this]{ Retry(); } );
		btnQuit->SetReleaseEvent( [this]{ Quit(); } );
	}
}

void PQScoreScreen::Update()
{
	
}

//void PQScoreScreen::Draw()
//{
//	Camera::activeCamera(camera);
//
//	scoreBackground->Draw();
//	if(btnRetry) btnRetry->Draw();
//	if(btnNext)  btnNext->Draw();
//	if(btnQuit)  btnQuit->Draw();
//}

void PQScoreScreen::OnTouchDown(float x, float y, int id)
{
	const float wx = camera->ScreenToWorldX(x);
	const float wy = camera->ScreenToWorldX(y);

	if(btnRetry) btnRetry->TryPress(wx, wy, id);
	if(btnNext)  btnNext->TryPress(wx, wy, id);
	if(btnQuit)  btnQuit->TryPress(wx, wy, id);
}

void PQScoreScreen::OnTouchUp(float x, float y, int id)
{
	const float wx = camera->ScreenToWorldX(x);
	const float wy = camera->ScreenToWorldX(y);

	if(btnRetry) btnRetry->TryRelease(id);
	if(btnNext)  btnNext->TryRelease(id);
	if(btnQuit)  btnQuit->TryRelease(id);
}

void PQScoreScreen::OnBackPressed()
{
	Quit();
}
