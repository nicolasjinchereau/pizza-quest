/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQMenu.h"
#include "PQCredits.h"
#include "PQGameLoader.h"
#include "Audio.h"
#include "Keycodes.h"
#include "PlayerProfile.h"
#include "Math.h"
#include "curves.h"
#include "PizzaQuest.h"

PQMenu::PQMenu()
{
	
}

PQMenu::~PQMenu()
{
	
}

void PQMenu::Start()
{
	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);

	const float cx = camera->NormalizedToWorldX(0.5f);
	const float cy = camera->NormalizedToWorldY(0.5f);

	background = AddChild(make_shared<Sprite>());

	background->Open("assets\\Images\\Menus\\Background.png");
	background->SetPos(cx, cy);
	background->layer = DrawLayer::Background;

	string continueBtnImg;

	if(PlayerProfile::currentLevel() == 0)
	{
		continueBtnImg = "assets\\Images\\Menus\\btnContinueDisabled.png";
	}
	else
	{
		continueBtnImg = "assets\\Images\\Menus\\btnContinue.png";
	}
	
	btnContinue = AddChild(make_shared<Button>());
	btnNew = AddChild(make_shared<Button>());
	btnCredits = AddChild(make_shared<Button>());
	btnQuit = AddChild(make_shared<Button>());

	btnContinue->Open(continueBtnImg.c_str());
	btnNew->Open("assets\\Images\\Menus\\btnNewGame.png");
	btnCredits->Open("assets\\Images\\Menus\\btnCredits.png");
	btnQuit->Open("assets\\Images\\Menus\\btnQuit.png");
	
	btnContinue->image()->layer = DrawLayer::UserInterface;
	btnNew->image()->layer = DrawLayer::UserInterface;
	btnCredits->image()->layer = DrawLayer::UserInterface;
	btnQuit->image()->layer = DrawLayer::UserInterface;

	btnContinue->SetPressEvent([]{ PizzaQuest::sounds().button->Play(); });
	btnNew->SetPressEvent([]{ PizzaQuest::sounds().button->Play(); });
	btnCredits->SetPressEvent([]{ PizzaQuest::sounds().button->Play(); });
	btnQuit->SetPressEvent([]{ PizzaQuest::sounds().button->Play(); });

	btnContinue->SetReleaseEvent([this]{ Continue(); });
	btnNew->SetReleaseEvent([this]{ NewGame(); });
	btnCredits->SetReleaseEvent([this]{ Credits(); });
	btnQuit->SetReleaseEvent([this]{ Quit(); });
	
	didClick = false;

	PizzaQuest::sounds().themeSong.Play();

	swipeInRoutine = RunCoroutine([=](yield_token<float> yield)
	{
		float xStart = -0.2f;
		float xFinish = 0.2f;
		float dist = xFinish - xStart;

		btnContinue->SetPosition(xStart, 0.175f);
		btnNew->SetPosition(xStart, 0.40625f);
		btnCredits->SetPosition(xStart, 0.6375f);
		btnQuit->SetPosition(xStart, 0.877083f);

		yield(0.5f);

		float length = 1.8f;
		float start = Time::time();
		float finish = start + length;

		auto getT = [](float elapsed, float start, float finish)
		{
			return (math::clamp(elapsed, start, finish) - start) / (finish - start);
		};

		while(Time::time() < finish)
		{
			float elapsed = Time::time() - start;
			float t0 = getT(elapsed, 0.0f, 0.6f);
			float t1 = getT(elapsed, 0.05f, 0.8f);
			float t2 = getT(elapsed, 0.1f, 1.2f);
			float t3 = getT(elapsed, 0.15f, 1.8f);

			float x0 = xStart + crv::out_dec_inv(t0) * dist;
			float x1 = xStart + crv::out_dec_inv(t1) * dist;
			float x2 = xStart + crv::out_dec_inv(t2) * dist;
			float x3 = xStart + crv::out_dec_inv(t3) * dist;

			btnContinue->SetPosition(x0, 0.175f);
			btnNew->SetPosition(x1, 0.40625f);
			btnCredits->SetPosition(x2, 0.6375f);
			btnQuit->SetPosition(x3, 0.877083f);

			yield(0);
		}

		btnContinue->SetPosition(xFinish, 0.175f);
		btnNew->SetPosition(xFinish, 0.40625f);
		btnCredits->SetPosition(xFinish, 0.6375f);
		btnQuit->SetPosition(xFinish, 0.877083f);
	});
}

void PQMenu::Continue()
{
	if(PlayerProfile::currentLevel() > 0)
	{
		DoButtonClick([]
		{
			PizzaQuest::sounds().themeSong.Stop();
			Engine::SetState( make_shared<PQGameLoader>() );
		});
	}
}

void PQMenu::NewGame()
{
	DoButtonClick([]
	{
		PizzaQuest::sounds().themeSong.Stop();
		PlayerProfile::ResetStats();
		Engine::SetState( make_shared<PQGameLoader>() );
	});
}

void PQMenu::Credits()
{
	DoButtonClick([]
	{
		Engine::SetState( make_shared<PQCredits>() );
	});
}

void PQMenu::Quit()
{
	DoButtonClick([]
	{
		PizzaQuest::sounds().themeSong.Stop();
		Engine::QuitGame();
	});
}

void PQMenu::Update()
{

}

void PQMenu::DoButtonClick(function<void()> then)
{
	didClick = true;

	if(auto task = swipeInRoutine.lock())
		CancelTask(task);

	RunCoroutine([=](yield_token<float> yield)
	{
		vec2f pos0 = btnContinue->position();
		vec2f pos1 = btnNew->position();
		vec2f pos2 = btnCredits->position();
		vec2f pos3 = btnQuit->position();

		float velocity0 = 0;
		float velocity1 = 0;
		float velocity2 = 0;
		float velocity3 = 0;

		float accelleration0 = 3.0f;
		float accelleration1 = 4.0f;
		float accelleration2 = 5.0f;
		float accelleration3 = 6.0f;

		while(pos0.y < 1.2f)
		{
			float dt = Time::deltaTime();
			velocity0 += accelleration0 * dt;
			velocity1 += accelleration1 * dt;
			velocity2 += accelleration2 * dt;
			velocity3 += accelleration3 * dt;

			pos0.y += velocity0 * dt;
			pos1.y += velocity1 * dt;
			pos2.y += velocity2 * dt;
			pos3.y += velocity3 * dt;
			
			btnContinue->SetPosition(pos0.x, pos0.y);
			btnNew->SetPosition(pos1.x, pos1.y);
			btnCredits->SetPosition(pos2.x, pos2.y);
			btnQuit->SetPosition(pos3.x, pos3.y);

			yield(0);
		}

		then();
	});
}

void PQMenu::OnTouchDown(float x, float y, int id)
{
	if(didClick) return;

	const float wx = camera->ScreenToWorldX(x);
	const float wy = camera->ScreenToWorldX(y);
	
	btnContinue->TryPress(wx, wy, id);
	btnNew->TryPress(wx, wy, id);
	btnCredits->TryPress(wx, wy, id);
	btnQuit->TryPress(wx, wy, id);
}

void PQMenu::OnTouchUp(float x, float y, int id)
{
	if(didClick) return;

	const float wx = camera->ScreenToWorldX(x);
	const float wy = camera->ScreenToWorldX(y);

	btnContinue->TryRelease(id);
	btnNew->TryRelease(id);
	btnCredits->TryRelease(id);
	btnQuit->TryRelease(id);
}

void PQMenu::OnKeyUp(KeyCode keyCode)
{
	switch(keyCode)
	{
	case KeyCode::Num1:
		PlayerProfile::SetCurrentLevel(0);
		break;

	case KeyCode::Num2:
		PlayerProfile::SetCurrentLevel(1);
		break;

	case KeyCode::Num3:
		PlayerProfile::SetCurrentLevel(2);
		break;

	case KeyCode::Num4:
		PlayerProfile::SetCurrentLevel(3);
		break;
		
	case KeyCode::Num5:
		PlayerProfile::SetCurrentLevel(4);
		break;

	case KeyCode::Num6:
		PlayerProfile::SetCurrentLevel(5);
		break;

	default:
		return;
	}

	PizzaQuest::sounds().themeSong.Stop();
	Engine::SetState( make_shared<PQGameLoader>() );
}
