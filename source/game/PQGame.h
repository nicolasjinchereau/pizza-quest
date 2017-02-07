/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "bytestream.h"
#include "Engine.h"
#include "Time.h"
#include "Stream.h"
#include "Keycodes.h"
#include "State.h"
#include "Graph.h"
#include "PlayerProfile.h"
#include "Camera.h"
#include "Button.h"
#include "ParticleSystem.h"
#include "PizzaQuest.h"
#include "PQStillImage.h"
#include "PQGameTypes.h"
#include "PQDelivery.h"
#include "PQPizzaPickup.h"
#include "PQStructure.h"
#include "PQProp.h"
#include "PQTile.h"
#include "PQPizzaShop.h"
#include "PQCharacter.h"
#include "PQPedestrian.h"
#include "PQPlayer.h"
#include "PQVehicle.h"
#include "PQCopCar.h"
#include "PQProgressBar.h"
#include "PQJoystick.h"
#include "PQGameTimer.h"
#include "PQCompass.h"
#include "PQNitroGauge.h"
#include "PQHealthBar.h"
#include "PQDeliveryStatus.h"
#include "PQStrikeCounter.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cstring>
#include <coroutine.h>
using namespace coroutines;

class PQGame : public State
{
public:
	enum class GameResult
	{
		Win,
		Dead,
		TimeUp,
		Jail,
	};

	PQGame();
	~PQGame();

	void Initialize(yield_token<float> yield);
	int OpenMap(const char *filename, yield_token<float> yield, int loadTaskCount);
	void TryYield(yield_token<float> yield);

////////////////////////////////////

	virtual void Update();
	virtual void StateDraw() override;
	virtual void OnTouchDown(float x, float y, int id);
	virtual void OnTouchMove(float x, float y, int id);
	virtual void OnTouchUp(float x, float y, int id);
	virtual void OnBackPressed();
	
	virtual void OnKeyDown(KeyCode keyCode);
	virtual void OnKeyUp(KeyCode keyCode);

////////////////////////////////////

	bool movingLeft;
	bool movingRight;
	bool movingUp;
	bool movingDown;
	bool gameRunning;
	//int strikes;
	float progress;
	float nextYield;
	
	bool showingHelp;

	enum class TextMessage
	{
		FindYourDelivery,
		FindTheRestaurant,
		DontBeLate,
		WatchForCars,
		WatchForPeople,
		GoodJob,
	};

	void ShowMessage(TextMessage message);
	void ShowHelperArrow(const vec2f &center,
						 const vec2f &target,
						 float duration = 5.0f,
						 float bounces = 3.0f);

	void StartGame();
	void FinishGame(GameResult result);
	void DoPickup();
	void FinishPickup();
	void DoDelivery();
	void FinishDelivery();
	void DoExplosion(const vec2f &position);

	enum class StrikeReason
	{
		HitPerson,
		HitCar,
	};

	void AddStrike(StrikeReason reason);

////////////////////////////////////

	static const uint32_t mainMask = 1;
	static const uint32_t guiMask = 1 << 1;

	// don't need Object updates
	LevelData *pLevelData;
	string mapFilename;
	uint8_t mapVers;
	float timeLimit;
	int numDeliveries;
	float minPlayerX;
	float maxPlayerX;
	float minPlayerY;
	float maxPlayerY;
	float cameraScaleVelocity;
	Graph pedGraph;
	Graph vehGraph;
	vector<GraphNode*> pedGoals; // references
	vector<GraphNode*> vehGoals; // references
	vector<shared_ptr<PQResource>> resources;

	shared_ptr<Shader> particleShader;
	shared_ptr<ParticleSystem> smoke;
	shared_ptr<ParticleSystem> fire;
	shared_ptr<ParticleSystem> rubble1;
	shared_ptr<ParticleSystem> rubble2;
	shared_ptr<Camera> mainCamera;
	shared_ptr<Camera> guiCamera;
	shared_ptr<PQPlayer> player;
	shared_ptr<RigidBody> mapBordersLeft;
	shared_ptr<RigidBody> mapBordersRight;
	shared_ptr<RigidBody> mapBordersTop;
	shared_ptr<RigidBody> mapBordersBottom;
	vector<shared_ptr<Sprite>> pizzaIcons;
	shared_ptr<PQPizzaShop> pizzaShop;
	shared_ptr<PQPizzaPickup> pizzaPickup;
	vector<shared_ptr<PQTile>> tiles;
	vector<shared_ptr<PQStructure>> structures;
	vector<shared_ptr<PQProp>> props;
	list<shared_ptr<PQDelivery>> deliveries;
	vector<shared_ptr<PQVehicle>> cars;
	vector<shared_ptr<PQCharacter>> characters;
	vector<shared_ptr<PQCopCar>> npcCars;
	shared_ptr<PQGameTimer> timer;
	shared_ptr<PQCompass> compass;
	shared_ptr<PQNitroGauge> nitroGauge;
	shared_ptr<PQHealthBar> healthBar;
	shared_ptr<PQDeliveryStatus> deliveryStatus;
	shared_ptr<PQStrikeCounter> strikeCounter;
	shared_ptr<Sound> gameWin;
	shared_ptr<Sound> gameLoss;
	shared_ptr<Sound> sndDeliveryComplete;
	shared_ptr<Sound> phoneRing;
	shared_ptr<Sound> sndGotPizza;
	vector<shared_ptr<Sound>> customers;
	shared_ptr<Texture> msgFindYourDelivery;
	shared_ptr<Texture> msgFindTheRestaurant;
	shared_ptr<Texture> msgDontBeLate;
	shared_ptr<Texture> msgWatchForCars;
	shared_ptr<Texture> msgWatchForPeople;
	shared_ptr<Texture> msgGoodJob;
	shared_ptr<Sprite> helperArrow;
	shared_ptr<Stream> mainSong;
};
