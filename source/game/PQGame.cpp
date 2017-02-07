/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQGame.h"
#include "PQPauseMenu.h"
#include "PQScoreScreen.h"
#include "PQCredits.h"
#include "utils.h"
#include "Audio.h"
#include "PQPowerUp.h"
#include "curves.h"
#include "Math.h"
#include "PlayerProfile.h"
#include <algorithm>
using namespace std;

PQGame::PQGame()
{
	movingLeft = false;
	movingRight = false;
	movingUp = false;
	movingDown = false;
	pLevelData = nullptr;
	mapVers = 0;
	timeLimit = 0;
	numDeliveries = 0;
	minPlayerX = 0;
	maxPlayerX = 0;
	minPlayerY = 0;
	maxPlayerY = 0;
	nextYield = 0;
	progress = 0;
	gameRunning = false;
	showingHelp = false;
	cameraScaleVelocity = 0;

	srand((unsigned int)time(nullptr));
}

PQGame::~PQGame()
{
	
}

void PQGame::OnBackPressed()
{
	if(!gameRunning)
		return;

	auto game = static_pointer_cast<PQGame>(shared_from_this());
	auto pauseMenu = make_shared<PQPauseMenu>(game);
	Engine::PushState(pauseMenu);
}

void PQGame::OnKeyDown(KeyCode keyCode)
{
	switch(keyCode)
	{
	case KeyCode::A:
		movingLeft = true;
		break;
	case KeyCode::S:
		movingDown = true;
		break;
	case KeyCode::D:
		movingRight = true;
		break;
	case KeyCode::W:
		movingUp = true;
		break;
	case KeyCode::E:

		break;
	}
}

void PQGame::OnKeyUp(KeyCode keyCode)
{
	switch(keyCode)
	{
	case KeyCode::A:
		movingLeft = false;
		break;
	case KeyCode::S:
		movingDown = false;
		break;
	case KeyCode::D:
		movingRight = false;
		break;
	case KeyCode::W:
		movingUp = false;
		break;
	case KeyCode::E:
		if(gameRunning) player->ToggleInCar();
		break;
	case KeyCode::Space:

		break;
	}
}

void PQGame::Initialize(yield_token<float> yield)
{
	mainCamera = AddChild(make_shared<Camera>());
	mainCamera->SetSize(480);

	guiCamera = AddChild(make_shared<Camera>());
	guiCamera->SetSize(480);
	guiCamera->ResetView();

	vector<function<void()>> loaderTasks;
	loaderTasks.reserve(64);

	//joystick = AddChild(make_shared<PQJoystick>());
	//joystick->GetLoadTasks(loaderTasks);

	///////

	//loaderTasks.emplace_back([this]{
	//	useButton = AddChild(make_shared<Button>());
	//	useButton->Open("assets\\Images\\GUI\\ActionButton.png");
	//	useButton->SetPosition(0.87f, 0.87f);
	//	useButton->SetReleaseEvent( [this]{ ToggleInCar(); } );
	//});

	healthBar = AddChild(make_shared<PQHealthBar>());
	healthBar->normalizedPosition(vec2f(0.22f, 0.06f));
	healthBar->fill() = 1.0f;
	healthBar->GetLoadTasks(loaderTasks);

	deliveryStatus = AddChild(make_shared<PQDeliveryStatus>());
	deliveryStatus->normalizedPosition(vec2f(0.64f, 0.065f));
	deliveryStatus->GetLoadTasks(loaderTasks);

	nitroGauge = AddChild(make_shared<PQNitroGauge>());
	nitroGauge->normalizedPosition(vec2f(0.78f, 0.865f));
	nitroGauge->psi(0.0f);
	nitroGauge->GetLoadTasks(loaderTasks);

	compass = AddChild(make_shared<PQCompass>());
	compass->normalizedPosition(vec2f(0.92f, 0.865f));
	compass->GetLoadTasks(loaderTasks);

	strikeCounter = AddChild(make_shared<PQStrikeCounter>());
	strikeCounter->normalizedPosition(vec2f(0.04f, 0.94f));
	strikeCounter->GetLoadTasks(loaderTasks);

	timer = AddChild(make_shared<PQGameTimer>());
	timer->normalizedPosition(vec2f(0.5f, 0.07f));
	timer->GetLoadTasks(loaderTasks);

	loaderTasks.emplace_back([this]{
		gameWin = AddChild(make_shared<Sound>());
		gameWin->Open("assets\\Sounds\\Effects\\gameWin.wav");
	});

	loaderTasks.emplace_back([this]{
		gameLoss = AddChild(make_shared<Sound>());
		gameLoss->Open("assets\\Sounds\\Effects\\gameLoss.wav");
	});

	loaderTasks.emplace_back([this]{
		sndDeliveryComplete = AddChild(make_shared<Sound>());
		sndDeliveryComplete->Open("assets\\Sounds\\Effects\\deliveryComplete.wav");
	});

	loaderTasks.emplace_back([this]{
		phoneRing = AddChild(make_shared<Sound>());
		phoneRing->Open("assets\\Sounds\\Effects\\phonering.wav");
		phoneRing->SetGain(0.2f);
	});

	loaderTasks.emplace_back([this]{
		sndGotPizza = AddChild(make_shared<Sound>());
		sndGotPizza->Open("assets\\Sounds\\Effects\\gotPizza.wav");
	});

	loaderTasks.emplace_back([this]{
		mainSong = make_shared<Stream>();
		mainSong->Open(PlayerProfile::currentLevelData().songFilename.c_str());
		mainSong->SetLoop(true);
		mainSong->SetGain(0.5f);
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer01.wav");
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer02.wav");
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer03.wav");
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer04.wav");
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer05.wav");
	});

	loaderTasks.emplace_back([this]{
		customers.emplace_back(AddChild(make_shared<Sound>()));
		customers.back()->Open("assets\\Sounds\\Effects\\customer06.wav");
	});

	loaderTasks.emplace_back([this]{
		msgFindYourDelivery = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgFindYourDelivery.png"));
	});

	loaderTasks.emplace_back([this]{
		msgFindTheRestaurant = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgFindTheRestaurant.png"));
	});

	loaderTasks.emplace_back([this]{
		msgDontBeLate = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgDontBeLate.png"));
	});

	loaderTasks.emplace_back([this]{
		msgWatchForCars = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgWatchForCars.png"));
	});

	loaderTasks.emplace_back([this]{
		msgWatchForPeople = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgWatchForPeople.png"));
	});

	loaderTasks.emplace_back([this]{
		msgGoodJob = AddChild(make_shared<Texture>("assets\\Images\\GUI\\msgGoodJob.png"));
	});

	loaderTasks.emplace_back([this]{
		helperArrow = AddChild(make_shared<Sprite>("assets\\Images\\GUI\\Arrow.png"));
		helperArrow->SetVisible(false);
		helperArrow->category = 2;
		helperArrow->layer = DrawLayer::UserInterface + 100;
	});
	
	struct scale_limiter
	{
		inline float low() { return 1.0f; }
		inline float high() { return 4.0f; }
	};

	loaderTasks.emplace_back([this]{
		particleShader = AddChild(make_shared<Shader>("assets\\Shaders\\painted.vert",
													  "assets\\Shaders\\painted.frag"));
	});

	loaderTasks.emplace_back([this]{
		smoke = AddChild(make_shared<ParticleSystem>());
		smoke->category = 1;
		smoke->layer = DrawLayer::Props + 100;
		smoke->SetTexture(make_shared<Texture>("assets\\Images\\Particles\\smoke.png"));
		smoke->SetShader(particleShader);
		smoke->SetMaxParticles(50);
		smoke->SetRadius(20);
		smoke->SetScale(0.8f, 0.3f, crv::limit<crv::in_linear, scale_limiter>);
		smoke->SetSpeed(20, 20, crv::in_quad_inv);
		smoke->SetLife(3.0f);
		smoke->SetAlpha(0.4f, 0.1f, crv::inout<crv::out_dec_inv, crv::out_quad_inv>);
		smoke->SetAngularVelocity(10, 5, crv::in_cube_inv);
	});

	loaderTasks.emplace_back([this]{
		fire = AddChild(make_shared<ParticleSystem>());
		fire->category = 1;
		fire->layer = DrawLayer::Props + 100;
		fire->SetTexture(make_shared<Texture>("assets\\Images\\Particles\\fire.png"));
		fire->SetShader(particleShader);
		fire->SetMaxParticles(50);
		fire->SetRadius(60);
		fire->SetScale(2.0f, 0.7f);
		fire->SetSpeed(10, 4, crv::in_quad_inv);
		fire->SetLife(0.1f);
		fire->SetAlpha(0.7f, 0.1f, crv::inout<crv::out_dec_inv, crv::in_linear>);
		fire->SetAngularVelocity(40, 10, crv::in_cube_inv);
	});

	loaderTasks.emplace_back([this]{
		rubble1 = AddChild(make_shared<ParticleSystem>());
		rubble1->category = 1;
		rubble1->layer = DrawLayer::Props + 100;
		rubble1->SetTexture(make_shared<Texture>("assets\\Images\\Particles\\rubble1.png"));
		rubble1->SetShader(particleShader);
		rubble1->SetMaxParticles(50);
		rubble1->SetRadius(10);
		rubble1->SetScale(0.07f, 0.03f);
		rubble1->SetSpeed(500, 460, crv::in_cube_inv);
		rubble1->SetLife(3.0f);
		rubble1->SetAlpha(1.0f, 0.0f, crv::in_icosa_inv);
		rubble1->SetAngularVelocity(360, 180, crv::in_quad_inv);
	});

	loaderTasks.emplace_back([this]{
		rubble2 = AddChild(make_shared<ParticleSystem>());
		rubble2->category = 1;
		rubble2->layer = DrawLayer::Props + 100;
		rubble2->SetTexture(make_shared<Texture>("assets\\Images\\Particles\\rubble2.png"));
		rubble2->SetShader(particleShader);
		rubble2->SetMaxParticles(50);
		rubble2->SetRadius(10);
		rubble2->SetScale(0.07f, 0.03f);
		rubble2->SetSpeed(500, 460, crv::in_cube_inv);
		rubble2->SetLife(3.0f);
		rubble2->SetAlpha(1.0f, 0.0f, crv::in_icosa_inv);
		rubble2->SetAngularVelocity(360, 180, crv::in_quad_inv);
	});

	//////////////////////////

	progress = 0.0f;
	OpenMap(PlayerProfile::currentLevelData().mapFilename.c_str(), yield, loaderTasks.size());
	
	float remaining = 1.0f - progress;
	float progressPerItem = remaining / (float)loaderTasks.size();

	for(auto& item : loaderTasks)
	{
		item();

		progress += progressPerItem;
		TryYield(yield);
	}

	pLevelData = &PlayerProfile::currentLevelData();
	pLevelData->ClearScore();
	pLevelData->deliveriesAssigned = numDeliveries;
	pLevelData->timeLimit = timeLimit;
	pLevelData->health = 100;

	StartGame();
}

void PQGame::StartGame()
{
	deliveryStatus->deliveryCount(numDeliveries);

	Time::Reset();

	DoPickup();

	timer->SetTimeLimit(timeLimit);
	timer->StartTimer();
		
	gameRunning = true;
}

void PQGame::FinishGame(GameResult result)
{
	if(!gameRunning)
		return;

	gameRunning = false;

	RunCoroutine([this, result](yield_token<float> yield)
	{
		mainSong->Stop();
		
		movingLeft = false;
		movingRight = false;
		movingUp = false;
		movingDown = false;

		player->enabled() = false;

		timer->PauseTimer();

		yield(4.0f);

		string fnResultImage;
		string fnResultSound;
		bool finishedGame = false;

		switch(result)
		{
		case GameResult::Win:
			if(PlayerProfile::currentLevel() == PlayerProfile::levelCount() - 1)
			{
				fnResultImage = "assets\\Images\\Title Screens\\Success_Chick.png";
				fnResultSound = "assets\\Sounds\\Effects\\gameWin.wav";
				finishedGame = true;
			}
			else
			{
				fnResultImage = "assets\\Images\\Title Screens\\Success_Money.png";
				fnResultSound = "assets\\Sounds\\Effects\\gameWin.wav";
			}
			break;

		case GameResult::Dead:
			fnResultImage = "assets\\Images\\Title Screens\\GameOver_Dead.png";
			fnResultSound = "assets\\Sounds\\Effects\\gameLoss.wav";
			break;

		case GameResult::TimeUp:
			fnResultImage = "assets\\Images\\Title Screens\\GameOver_Late.png";
			fnResultSound = "assets\\Sounds\\Effects\\gameLoss.wav";
			break;

		case GameResult::Jail:
			fnResultImage = "assets\\Images\\Title Screens\\GameOver_Jail.png";
			fnResultSound = "assets\\Sounds\\Effects\\gameLoss.wav";
			break;
		}

		// fill out description of result screen
		StillImageDesc resultImgDesc;

		resultImgDesc.fadeInLength = 0.4f;
		resultImgDesc.sustainLength = 4.0f;
		resultImgDesc.fadeOutLength = 0.6f;
		resultImgDesc.fnImage = fnResultImage;
		resultImgDesc.fnSound = fnResultSound;
		resultImgDesc.tapOut = true;
		resultImgDesc.callback = [finishedGame]()
		{
			if(finishedGame)
				Engine::SetState( make_shared<PQCredits>() );
			else
				Engine::SetState( make_shared<PQScoreScreen>() );
		};
		
		pLevelData->health = max((int)player->health(), 0);
		pLevelData->timeCompleted = min(timer->timeElapsed(), timeLimit);
		pLevelData->deliveriesCompleted = numDeliveries - deliveries.size();
		pLevelData->Evalutate();

		Engine::SetState(make_shared<PQStillImage>(resultImgDesc));
	});
}

void PQGame::DoPickup()
{
	RunCoroutine([&](yield_token<float> yield)
	{
		yield(0);

		phoneRing->Play();
		yield(phoneRing->length() * 0.8f);

		auto& snd = customers[rand() % customers.size()];
		snd->Play();

		yield(snd->length());

		pizzaPickup->Enable(true);

		compass->ShowGreenArrow();
		compass->SetDestination(pizzaPickup->position);
		
		if(deliveryStatus->deliveriesCompleted() == 0)
		{
			if(PlayerProfile::currentLevel() == 0)
				ShowMessage(PQGame::TextMessage::FindTheRestaurant);
			
			yield(1.0f);

			mainSong->Play();
		}
	});
}

void PQGame::FinishPickup()
{
	RunAfterUpdate([&]
	{
		player->GivePizza();
		sndGotPizza->Play();

		vec2f pos = pizzaPickup->body->pixelPosition();
		pos = mainCamera->WorldToScreen(pos);
		deliveryStatus->AddReadyPizza(pos);

		pizzaPickup->Enable(false);
		compass->ShowNoArrow();

		DoDelivery();
	});
}

void PQGame::DoDelivery()
{
	RunAfterUpdate([&]
	{
		auto nextDelivery = deliveries.back();
		nextDelivery->Enable(true);
		
		compass->ShowRedArrow();
		compass->SetDestination(nextDelivery->position);

		if(PlayerProfile::currentLevel() == 0)
		{
			ShowMessage(TextMessage::FindYourDelivery);
		}
	});
}

void PQGame::FinishDelivery()
{
	RunCoroutine([&](yield_token<float> yield)
	{
		yield(0);

		vec2f pos = deliveries.back()->body->pixelPosition();
		pos = mainCamera->WorldToScreen(pos);
		deliveryStatus->AddCompletedDelivery(pos);

		deliveries.back()->Enable(false);
		deliveries.pop_back();

		player->TakePizza();
		sndDeliveryComplete->Play();

		ShowMessage(PQGame::TextMessage::GoodJob);

		compass->ShowNoArrow();

		yield(2.0f);

		if(!deliveries.empty())
		{
			DoPickup();
		}
	});
}

void PQGame::DoExplosion(const vec2f &position)
{
	PizzaQuest::sounds().explosion->Play();

	smoke->position(position);
	fire->position(position);
	rubble1->position(position);
	rubble2->position(position);

	smoke->Spawn(10);
	fire->Spawn(10);
	rubble1->Spawn(20);
	rubble2->Spawn(20);
}

void PQGame::AddStrike(StrikeReason reason)
{
	strikeCounter->AddStrike();

	switch(reason)
	{
	case StrikeReason::HitPerson:
		healthBar->DoBlackPulse();
		player->health() -= 20;
		healthBar->fill() = player->health() / 100.0f;

		ShowMessage(PQGame::TextMessage::WatchForPeople);
		break;
	
	case StrikeReason::HitCar:
		healthBar->DoRedPulse();
		player->health() -= 30;
		healthBar->fill() = player->health() / 100.0f;

		ShowMessage(PQGame::TextMessage::WatchForCars);
		break;
	}
}

void PQGame::ShowMessage(TextMessage message)
{
	shared_ptr<Texture> tex;

	switch(message)
	{
	case TextMessage::FindYourDelivery:
		tex = msgFindYourDelivery;

		RunCoroutine([=](yield_token<float> yield)
		{
			yield(2.0f);
			
			vec2f pos = guiCamera->NormalizedToWorld(compass->normalizedPosition());
			ShowHelperArrow(pos + vec2f(-40, -120), pos, 3.0f, 4.0f);
		});

		break;

	case TextMessage::FindTheRestaurant:
		tex = msgFindTheRestaurant;

		RunCoroutine([=](yield_token<float> yield)
		{
			yield(2.0f);

			vec2f pos = guiCamera->NormalizedToWorld(compass->normalizedPosition());
			ShowHelperArrow(pos + vec2f(-40, -120), pos, 3.0f, 4.0f);
		});
		break;

	case TextMessage::DontBeLate:
		tex = msgDontBeLate;
		break;

	case TextMessage::WatchForCars:
		tex = msgWatchForCars;
		break;

	case TextMessage::WatchForPeople:
		tex = msgWatchForPeople;
		break;

	case TextMessage::GoodJob:
		tex = msgGoodJob;
		break;
	}

	if(tex)
	{
		RunCoroutine([=](yield_token<float> yield)
		{
			shared_ptr<Sprite> sp = AddChild(make_shared<Sprite>(tex));
			sp->category = 2;
			sp->layer = DrawLayer::UserInterface + 100;

			float start = Time::time();
			float finish = Time::time() + 5.0f;

			while(Time::time() < finish)
			{
				float t = (Time::time() - start) / 5.0f;
				float ny = 1.1f - crv::in_elastic(math::clamp(t * 3, 0.0f, 1.0f)) * 0.2f;
				sp->SetPos(guiCamera->NormalizedToWorld(vec2f(0.5f, ny)));
				yield(0);
			}

			RemoveChild(sp);
		});
	}
}

void PQGame::ShowHelperArrow(const vec2f &center,
							 const vec2f &target,
							 float duration,
							 float bounces)
{
	if(showingHelp)
		return;

	RunCoroutine([=](yield_token<float> yield)
	{
		vec2f direction = (target - center).Normalized();

		float start = Time::time();
		float finish = start + duration;

		helperArrow->SetVisible(true);
		helperArrow->SetScale(0.5f);

		while(Time::time() <= finish)
		{
			float t = (Time::time() - start) / duration * bounces;
			
			const float bounceLength = 20;

			float offset = -cos(t * 2 * math::pi);
			offset *= bounceLength;
			vec2f pos = center + direction * offset;

			helperArrow->SetPos(pos);
			helperArrow->SetAngle(math::deg(atan2(direction.y, direction.x)));

			yield(0);
		}

		helperArrow->SetVisible(false);
	});
}

void PQGame::Update()
{
	if(gameRunning)
	{
		vec2f dir(0, 0);

		if(movingLeft)	dir.x -= 1.0f;
		if(movingRight) dir.x += 1.0f;
		if(movingUp)	dir.y -= 1.0f;
		if(movingDown)	dir.y += 1.0f;

		dir.Normalize();
		
		player->SetInput(dir);

		float sz_goal = 480;
		float sz_grow = 200;

		if(player->car)
		{
			vec2f vel = player->car->body->velocity();
			sz_goal += (vel.Length() / player->car->maxFrontSpeed) * 300;
		}

		float sz = mainCamera->GetSize();
		
		float accellerationScale = 2.0f;
		float velocityScale = 0.5f;

		float diffLen = abs(sz_goal - sz);
		cameraScaleVelocity += (sz_goal - sz) * Time::deltaTime() * accellerationScale;
		cameraScaleVelocity = math::clamp(cameraScaleVelocity, -diffLen, +diffLen);

		sz += cameraScaleVelocity * Time::deltaTime() * velocityScale;

		mainCamera->SetSize(sz);

		if(deliveries.empty())
		{
			FinishGame(GameResult::Win);
		}
		else if(!player->IsAlive())
		{
			FinishGame(GameResult::Dead);
		}
		else if(timer->timedOut())
		{
			FinishGame(GameResult::TimeUp);
		}
		else if(strikeCounter->strikeCount() >= 3)
		{
			FinishGame(GameResult::Jail);
		}
	}
}

void PQGame::StateDraw()
{
// DRAW GAME
	Camera::activeCamera(mainCamera);
	
	RenderQueue::Clear();
	
	SubmitDrawCalls_R(mainMask);

	RenderQueue::Sort();
	RenderQueue::Execute();

// DRAW USER INTERFACE
	Camera::activeCamera(guiCamera);

	RenderQueue::Clear();
	
	SubmitDrawCalls_R(guiMask);
	
	RenderQueue::Sort();
	RenderQueue::Execute();
}

void PQGame::OnTouchDown(float x, float y, int id)
{
	if(!gameRunning) return;

	/*const float guiWX = guiCamera->ScreenToWorldX(x);
	const float guiWY = guiCamera->ScreenToWorldX(y);*/

	//if(guiWX <= (guiCamera->viewRect.w  / 2.0f))
	//	joystick->Press(guiWX, guiWY, id);
	
	//useButton->TryPress(guiWX, guiWY, id);
}

void PQGame::OnTouchMove(float x, float y, int id)
{
	if(!gameRunning) return;

	//const float guiWX = guiCamera->ScreenToWorldX(x);
	//const float guiWY = guiCamera->ScreenToWorldX(y);
	
	//joystick->SetThumbPos(guiWX, guiWY, id);
}

void PQGame::OnTouchUp(float x, float y, int id)
{
	if(!gameRunning) return;

	//joystick->TryRelease(id);
	//useButton->TryRelease(id);
}

void PQGame::TryYield(yield_token<float> yield)
{
	if(Time::exactTime() > nextYield)
	{
		nextYield = Time::exactTime() + 0.03333f;
		yield(progress);
	}
}

int PQGame::OpenMap(const char *filename, yield_token<float> yield, int loadTaskCount)
{
	bytestream mapfile = bytestream_from_file(filename);

	if(mapfile.empty())
	{
		Trace("Could not open map file", filename);
		return -1;
	}
	
	mapFilename = filename;

	Path mapFolder = filename;
	mapFolder.remove_back();

//MAP VERSION
	mapfile.read((char*)&mapVers, sizeof(uint8_t));

// TIME LIMIT
	unsigned int oldTimeLimit;
	mapfile.read((char*)&oldTimeLimit, sizeof(uint32_t));
	timeLimit = (float)oldTimeLimit / 1000.0f; // time limit is now in seconds(float)

	mapfile.read((char*)&numDeliveries, sizeof(int));

// MAP BORDERS
	mapfile.read((char*)&minPlayerX, sizeof(float));
	mapfile.read((char*)&maxPlayerX, sizeof(float));
	mapfile.read((char*)&minPlayerY, sizeof(float));
	mapfile.read((char*)&maxPlayerY, sizeof(float));

	vec2f corner = Physics::toMeters(minPlayerX, minPlayerY);
	mapBordersLeft = make_shared<RigidBody>(physics, corner, 0.0f, RigidBody::Type::Static);
	mapBordersRight = make_shared<RigidBody>(physics, corner, 0.0f, RigidBody::Type::Static);
	mapBordersTop = make_shared<RigidBody>(physics, corner, 0.0f, RigidBody::Type::Static);
	mapBordersBottom = make_shared<RigidBody>(physics, corner, 0.0f, RigidBody::Type::Static);
	
	float mapHeight = Physics::toMeters(maxPlayerY - minPlayerY);
	float mapWidth = Physics::toMeters(maxPlayerX - minPlayerX);

	mapBordersLeft->AddEdge(vec2f(0, 0), vec2f(0, mapHeight), 1, 0, 0, 0);
	mapBordersRight->AddEdge(vec2f(mapWidth, 0), vec2f(mapWidth, mapHeight), 1, 0, 0, 0);
	mapBordersTop->AddEdge(vec2f(0, 0), vec2f(mapWidth, 0), 1, 0, 0, 0);
	mapBordersBottom->AddEdge(vec2f(0, mapHeight), vec2f(mapWidth, mapHeight), 1, 0, 0, 0);

	mapBordersLeft->self_mask(ContactMask::MapBorder);
	mapBordersLeft->others_mask(ContactMask::Player | ContactMask::Vehicle);
	mapBordersRight->self_mask(ContactMask::MapBorder);
	mapBordersRight->others_mask(ContactMask::Player | ContactMask::Vehicle);
	mapBordersTop->self_mask(ContactMask::MapBorder);
	mapBordersTop->others_mask(ContactMask::Player | ContactMask::Vehicle);
	mapBordersBottom->self_mask(ContactMask::MapBorder);
	mapBordersBottom->others_mask(ContactMask::Player | ContactMask::Vehicle);

	AddChild(mapBordersLeft);
	AddChild(mapBordersRight);
	AddChild(mapBordersTop);
	AddChild(mapBordersBottom);

// RESOURCES
	int nResources;
	mapfile.read((char*)&nResources, sizeof(int));

	int nObjects;
	mapfile.read((char*)&nObjects, sizeof(int));

	resources.reserve(nResources);
	
	float progressPerResource = 1.0f / (loadTaskCount + nResources + (nObjects / 10));
	float progressPerObject = progressPerResource / 10.0f;

	for(int i = 0; i < nResources; i++)
	{
		uint8_t type;
		mapfile.read((char*)&type, sizeof(uint8_t));

		switch(type)
		{
			case RES_IMAGE:
			{
				shared_ptr<PQResImage> rImg = make_shared<PQResImage>();
				
				rImg->type = type;
				
				mapfile.read((char*)&rImg->source_file, sizeof(rImg->source_file));
				Path fnRes(rImg->source_file);
				fnRes.make_absolute(mapFolder);
				strcpy_s(rImg->source_file, fnRes.c_str());
				
				mapfile.read((char*)&rImg->resource_name, sizeof(rImg->resource_name));
				mapfile.read((char*)&rImg->class_id, sizeof(rImg->class_id));
				mapfile.read((char*)&rImg->nRows, sizeof(uint16_t));
				mapfile.read((char*)&rImg->nCols, sizeof(uint16_t));

				rImg->Init();

				int nShapes;
				mapfile.read((char*)&nShapes, sizeof(int));
				
				for(int s = 0; s < nShapes; ++s)
				{
					uint8_t shapeType;
					mapfile.read((char*)&shapeType, sizeof(uint8_t));

					switch(shapeType)
					{
						case SHAPE_CIRCLE:
						{
							auto circle = make_shared<PQB2Circle>();
							mapfile.read((char*)&circle->center, sizeof(vec2f));
							mapfile.read((char*)&circle->radius, sizeof(float));
							rImg->collision_shapes.push_back(circle);
							break;
						}
						case SHAPE_POLYGON:
						{
							auto polygon = make_shared<PQB2Polygon>();
							mapfile.read((char*)&polygon->nVerts, sizeof(int));
							mapfile.read((char*)polygon->vertices, polygon->nVerts * sizeof(vec2f));
							rImg->collision_shapes.push_back(polygon);
							break;
						}
						case SHAPE_BOX:
						{
							auto box = make_shared<PQB2Box>();
							mapfile.read((char*)&box->center, sizeof(vec2f));
							mapfile.read((char*)&box->halfWidth, sizeof(float));
							mapfile.read((char*)&box->halfHeight, sizeof(float));
							mapfile.read((char*)&box->angle, sizeof(float));
							rImg->collision_shapes.push_back(box);
							break;
						}
						case SHAPE_EDGE:
						{
							auto edge = make_shared<PQB2Edge>();
							mapfile.read((char*)&edge->p1, sizeof(vec2f));
							mapfile.read((char*)&edge->p2, sizeof(vec2f));
							rImg->collision_shapes.push_back(edge);
							break;
						}
					}
				}

				resources.push_back(rImg);

				break;
			}
			case RES_SOUND:
			{
				shared_ptr<PQResSound> rSnd = make_shared<PQResSound>();

				mapfile.read((char*)&rSnd->source_file, sizeof(rSnd->source_file));
				mapfile.read((char*)&rSnd->resource_name, sizeof(rSnd->resource_name));
				mapfile.read((char*)&rSnd->class_id, sizeof(rSnd->class_id));

				Path fnRes(rSnd->source_file);
				fnRes.make_absolute(mapFolder);
				strcpy_s(rSnd->source_file, fnRes.c_str());
				
				rSnd->type = type;
				rSnd->Init();

				resources.push_back(rSnd);
				break;
			}
		}

		progress += progressPerResource;
		TryYield(yield);
	}

// MAP OBJECTS
	vector<shared_ptr<PQDelivery>> possibleDeliveries;

	for(int i = 0; i < nObjects; i++)
	{
		uint8_t type;

		mapfile.read((char*)&type, sizeof(uint8_t));

		switch(type)
		{
			case MAP_IMAGE:
			{
				uint8_t resIndex;
				vec2f position;
				uint8_t row;
				uint8_t col;
				float angle;
				float scale;
				int16_t value1;
				int16_t value2;

				mapfile.read((char*)&resIndex, sizeof(uint8_t));
				mapfile.read((char*)&position, sizeof(vec2f));
				mapfile.read((char*)&row, sizeof(uint8_t));
				mapfile.read((char*)&col, sizeof(uint8_t));
				mapfile.read((char*)&angle, sizeof(float));
				mapfile.read((char*)&scale, sizeof(float));
				mapfile.read((char*)&value1, sizeof(int16_t));
				mapfile.read((char*)&value2, sizeof(int16_t));

				shared_ptr<PQResource> resPtr = resources[resIndex];
				shared_ptr<PQMapImage> mapImg;

				string ClassID = resPtr->class_id;

				if(ClassID == "Tile")
				{
					auto tile = AddChild(make_shared<PQTile>());
					tiles.push_back(tile);
					mapImg = tile;
				}
				else if(ClassID == "Structure")
				{
					auto structure = AddChild(make_shared<PQStructure>());
					structures.emplace_back(structure);
					mapImg = structure;
				}
				else if(ClassID == "Prop")
				{
					auto prop = AddChild(make_shared<PQProp>());
					props.emplace_back(prop);
					mapImg = prop;
				}
				else if(ClassID == "Delivery")
				{
					auto delivery = make_shared<PQDelivery>();
					possibleDeliveries.push_back(delivery);
					mapImg = delivery;
				}
				else if(ClassID == "PlayerStart")
				{
					player = AddChild(make_shared<PQPlayer>(position));
					mapImg = make_shared<PQMapImage>(); // dummy
				}
				else if(ClassID == "PizzaShop")
				{
					pizzaShop = AddChild(make_shared<PQPizzaShop>());
					mapImg = pizzaShop;
				}
				else if(ClassID == "PizzaPickup")
				{
					pizzaPickup = AddChild(make_shared<PQPizzaPickup>());
					mapImg = pizzaPickup;
				}
				else if(ClassID == "Vehicle")
				{
					auto car = AddChild(make_shared<PQVehicle>());
					cars.emplace_back(car);
					mapImg = car;
				}
				else if(ClassID == "PQPedestrian"
					 || ClassID == "Pedestrian"
					 || ClassID == "Chaser")
				{
					auto pedestrian = AddChild(make_shared<PQPedestrian>(this));
					characters.push_back(pedestrian);
					mapImg = pedestrian;
				}
				else if(ClassID == "CopCar")
				{
					auto car = AddChild(make_shared<PQCopCar>(this));
					npcCars.push_back(car);
					mapImg = car;
				}
				else if(ClassID == "Health")
				{
					auto powerup = AddChild(make_shared<PQPowerUp>(PQPowerUp::Type::Health));
					mapImg = powerup;
				}
				else if(ClassID == "Speed")
				{
					auto powerup = AddChild(make_shared<PQPowerUp>(PQPowerUp::Type::Speed));
					mapImg = powerup;
				}
				else if(ClassID == "Time")
				{
					auto powerup = AddChild(make_shared<PQPowerUp>(PQPowerUp::Type::Time));
					mapImg = powerup;
				}

				if(mapImg)
				{
					mapImg->resource = resPtr;
					mapImg->type = type;
					mapImg->angle = angle;
					mapImg->scale = scale;
					mapImg->row = row;
					mapImg->col = col;
					mapImg->value1 = value1;
					mapImg->value2 = value2;
					mapImg->position = position;
					mapImg->resIndex = resIndex;
					mapImg->img = mapImg->AddChild(make_shared<Sprite>());
					mapImg->img->category = 1;
					mapImg->img->SetTexture(mapImg->GetResource()->tex);
					mapImg->img->SetNumRows(mapImg->GetResource()->nRows);
					mapImg->img->SetNumCols(mapImg->GetResource()->nCols);
					mapImg->img->SetPos(position);
					mapImg->img->SetAngle(angle);
					mapImg->img->SetScale(scale);
					mapImg->img->SetRow(row);
					mapImg->img->SetColumn(col);
				}
				else if(ClassID == "PlayerStart")
				{
					// player is a special case, not a PQMapImage
				}
				else
				{
					Trace("Tried to load unsupport map image: ", ClassID);
				}

				break;
			}
			case MAP_SOUND:
			{
				uint8_t resIndex;
				vec2f position;
				uint8_t volume;
				bool loop;
				uint16_t delay;
				uint16_t triggerRadius;

				mapfile.read((char*)&resIndex, sizeof(uint8_t));
				mapfile.read((char*)&position, sizeof(vec2f));
				mapfile.read((char*)&volume, sizeof(uint8_t));
				mapfile.read((char*)&loop, sizeof(bool));
				mapfile.read((char*)&delay, sizeof(uint16_t));
				mapfile.read((char*)&triggerRadius, sizeof(uint16_t));

				// removed. 
				// above code left for compatibility.

				break;
			}
		}

		progress += progressPerObject;
		TryYield(yield);
	}
	
// NUMBER OF PEDESTRIAN GRAPH NODES
	int nPedGraphNodes;
	mapfile.read((char*)&nPedGraphNodes, sizeof(int));

	vec2f position;
	bool isDestination;

// PEDESTRIAN GRAPH NODES
	for(int i = 0; i < nPedGraphNodes; i++)
	{
		mapfile.read((char*)&position, sizeof(vec2f));
		mapfile.read((char*)&isDestination, sizeof(bool));
		pedGraph.AddNode(position.x, position.y, isDestination);
	}

	TryYield(yield);

// PEDESTRIAN GRAPH ADJACENCY LISTS
	int nNeighbours;
	int neighbourIndex;

	for(int i = 0; i < nPedGraphNodes; i++)
	{
		mapfile.read((char*)&nNeighbours, sizeof(int));
		pedGraph[i]->neighbours.reserve(nNeighbours);

		for(int j = 0; j < nNeighbours; j++)
		{
			mapfile.read((char*)&neighbourIndex, sizeof(int));
			pedGraph.Connect(i, neighbourIndex);
		}
	}

	TryYield(yield);

// NUMBER OF VEHICLE GRAPH NODES
	int nVehGraphNodes;
	mapfile.read((char*)&nVehGraphNodes, sizeof(int));

// VEHICLE GRAPH NODES
	for(int i = 0; i < nVehGraphNodes; i++)
	{
		mapfile.read((char*)&position, sizeof(vec2f));
		mapfile.read((char*)&isDestination, sizeof(bool));
		vehGraph.AddNode(position.x, position.y, isDestination);
	}

	TryYield(yield);

// VEHICLE GRAPH ADJACENCY LISTS
	for(int i = 0; i < nVehGraphNodes; i++)
	{
		mapfile.read((char*)&nNeighbours, sizeof(int));
		vehGraph[i]->neighbours.reserve(nNeighbours);
		
		for(int j = 0; j < nNeighbours; j++)
		{
			mapfile.read((char*)&neighbourIndex, sizeof(int));
			vehGraph.Connect(i, neighbourIndex);
		}
	}

	TryYield(yield);

// CHOOSE RANDOM DELIVERIES FROM ALL THE DELIVERIES
	int maxDeliveries = min(numDeliveries, (int)possibleDeliveries.size());

	for(int i = 0; i < maxDeliveries; i++)
	{
		int rDelivery = rand() % possibleDeliveries.size();
		auto it = possibleDeliveries.begin() + rDelivery;
		
		auto delivery = AddChild(*it);
		possibleDeliveries.erase(it);

		deliveries.push_back(delivery);
	}

	TryYield(yield);

// SORT OUT GRAPH GOAL NODES
	for(int i = 0; i < pedGraph.Size(); i++)
	{
		if(pedGraph[i]->isDestination == true)
			pedGoals.push_back( pedGraph[i] );
	}

	for(int i = 0; i < vehGraph.Size(); i++)
	{
		if(vehGraph[i]->isDestination == true)
			vehGoals.push_back( vehGraph[i] );
	}

	return 0;
}
