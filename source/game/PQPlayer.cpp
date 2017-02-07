/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQPlayer.h"
#include "PQB2Shapes.h"
#include "PQGame.h"
#include "PQVehicle.h"
#include "Engine.h"
#include "Audio.h"

class RayCast : public b2RayCastCallback
{
public:
	int hits;

	RayCast() : hits(0){}
	
	virtual b2_float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									const b2Vec2& normal, b2_float32 fraction)
	{
		ContactMask mask = ContactMask::All & ~(ContactMask::Tile | ContactMask::Delivery | ContactMask::Pickup);
		ContactMask cat = (ContactMask)fixture->GetFilterData().categoryBits;
		
		if((int)(cat & mask))
			++hits;

		return 0;
	}
};

PQPlayer::PQPlayer(const vec2f &pos)
{
	this->position = pos;
}

PQPlayer::~PQPlayer()
{
	
}

void PQPlayer::Start()
{
	category = 1;
	layer = DrawLayer::Characters + 100;

	_health = 100;
	_hasPizza = false;
	_visible = true;
	_enabled = true;
	_carStarted = false;
	_driveSoundGain = 0;
	_idleGain = 0.4f;

	car = nullptr;
	animation = nullptr;

///////////// PLAYER SPRITE
	imgWithPizza = AddChild(make_shared<Sprite>());
	imgWithPizza->Open("assets\\Images\\Characters\\playerWithPizza.png");
	imgWithPizza->SetScale(0.55f);
	imgWithPizza->SetNumRows(8);
	imgWithPizza->SetNumCols(17);
	imgWithPizza->SetPos(position);
	imgWithPizza->category = 0;

	imgNoPizza = AddChild(make_shared<Sprite>());
	imgNoPizza->Open("assets\\Images\\Characters\\playerNoPizza.png");
	imgNoPizza->SetScale(0.55f);
	imgNoPizza->SetNumRows(8);
	imgNoPizza->SetNumCols(17);
	imgNoPizza->SetPos(position);
	imgNoPizza->category = 0;

	image = imgNoPizza.get();

	for(int y = 0; y < 8; y++)
	{
		animWithPizza[y] = AddChild(make_shared<Animation>());
		animNoPizza[y] = AddChild(make_shared<Animation>());

		animWithPizza[y]->sprite(imgWithPizza);
		animNoPizza[y]->sprite(imgNoPizza);

		for(int i = 1; i < 17; i++)
		{
			animWithPizza[y]->AddFrame(y, i);
			animNoPizza[y]->AddFrame(y, i);
		}
	}

	nAngles = imgNoPizza->GetNumRows();
	float degreesPerRow = 360.0f / (float)nAngles;
	angleTolerance = degreesPerRow / 2.0f;
	
	angles.reset(new float[imgNoPizza->GetNumRows()]);

	float ang = 0.0f;

	for(int i = 0; i < imgNoPizza->GetNumRows(); i++)
	{
		angles[i] = ang;
		ang += degreesPerRow;
	}

//////////// PARTICLES
	
	struct scale_limiter
	{
		inline float low() { return 1.0f; }
		inline float high() { return 2.0f; }
	};

	exhaust = AddChild(make_shared<ParticleSystem>());
	
	exhaust->SetTexture(make_shared<Texture>("assets\\Images\\Particles\\smoke.png"));
	exhaust->SetShader(make_shared<Shader>("assets\\Shaders\\painted.vert", "assets\\Shaders\\painted.frag"));
	exhaust->category = 1;
	exhaust->layer = DrawLayer::Tiles + 100;
	exhaust->SetMaxParticles(200);
	exhaust->SetScale(0.16f, 0.05f, crv::limit<crv::in_quad, scale_limiter>);
	exhaust->SetSpeed(5, 2, crv::in_dec_inv);
	exhaust->SetRate(30);
	exhaust->SetLife(0.5f);
	exhaust->SetAlpha(0.5f, 0.1f, crv::in_cube_inv);
	exhaust->SetAngularVelocity(90, 45, crv::in_quad_inv);
	exhaust->SetRotation(0, 360);
	exhaust->SetRadius(0);

///////////// SOUNDS
	enterCar = AddChild(make_shared<Sound>());
	enterCar->Open("assets\\Sounds\\Effects\\enterCar01.wav");

	exitCar = AddChild(make_shared<Sound>());
	exitCar->Open("assets\\Sounds\\Effects\\exitCar01.wav");
	
	carStart = AddChild(make_shared<Sound>());
	carStart->Open("assets\\Sounds\\Effects\\carStart.wav");
	carStart->SetGain(0.4f);

	carIdle = AddChild(make_shared<Sound>());
	carIdle->Open("assets\\Sounds\\Effects\\carIdle.wav");
	carIdle->SetLoop(true);

	carDrive = AddChild(make_shared<Sound>());
	carDrive->Open("assets\\Sounds\\Effects\\carDrive.wav");
	carDrive->SetLoop(true);

	nitrous = AddChild(make_shared<Sound>());
	nitrous->Open("assets\\Sounds\\Effects\\nitrous.wav");
	nitrous->SetGain(0.6f);

	float playerRadius = 20.0f;

	body = make_shared<RigidBody>(state()->physics,
					              Physics::toMeters(position),
					              0.0f,
					              RigidBody::Type::Dynamic);
	
	body->self_mask(ContactMask::Player);
	body->others_mask(ContactMask::All);
	body->AddCircle(vec2f::zero, Physics::toMeters(playerRadius), 1.0f, 0.1f, 0.02f, 0.0f);
	body->allowSleep(false);

	AddChild(body);

	state()->as<PQGame>()->mainCamera->CenterAt(position.x, position.y, true);
}

void PQPlayer::Update()
{
	if(!_enabled) return;
	
	if(!car)
	{
		float playerSpeed = 6.5f;

		vec2f velocity(input.x * playerSpeed,
					   input.y * playerSpeed);

		body->velocity(velocity);

		exhaust->emit(false);
	}
	else
	{
		if(_carStarted)
		{
			if(car->input.Length() > 0.1f)
				_driveSoundGain = min(_driveSoundGain + Time::deltaTime() * 1.0f, 1.0f);
			else
				_driveSoundGain = max(_driveSoundGain - Time::deltaTime() * 1.0f, 0.0f);
			
			carDrive->SetGain(_driveSoundGain * 0.3f);

			float pitchIncrease = car->body->velocity().Length() / car->maxFrontSpeed;
			carDrive->pitch(1.0f + pitchIncrease * 0.5f);
			carIdle->pitch(1.0f + pitchIncrease * 0.5f);

			struct scale_limiter
			{
				inline float low() { return 1.0f; }
				inline float high() { return 2.0f; }
			};

			int rate = (int)math::lerp(30.0f, 180.0f, _driveSoundGain);
			float scale = math::lerp(0.16f, 0.3f, _driveSoundGain);
			float scaleVariance = math::lerp(0.05f, 0.1f, _driveSoundGain);

			exhaust->emit(true);
			exhaust->SetRate(rate);
			exhaust->SetScale(scale, scaleVariance, crv::limit<crv::in_quad, scale_limiter>);
			exhaust->position(car->body->pixelPosition()
							+ car->body->up() * 34.0f
							+ car->body->right() * 12.0f);
			
			exhaust->SetForce(car->body->up() * 400);
		}
	}
}

void PQPlayer::LateUpdate()
{
	position = body->pixelPosition();

	if(!car)
	{
		auto game = static_pointer_cast<PQGame>(state());
		game->mainCamera->LookAt(position.x, position.y);
	}
}

void PQPlayer::Draw()
{
	if(!_visible) return;
	
	image->SetPos(position);
	
	if(animation)
		animation->Draw();
	else
		image->Draw();
}

vec2f PQPlayer::GetPosition()
{
	return car ? car->body->pixelPosition() : this->body->pixelPosition();
}

bool PQPlayer::IsAlive()
{
	return _health > 0;
}

bool PQPlayer::HasPizza()
{
	return _hasPizza;
}

void PQPlayer::GivePizza()
{
	_hasPizza = true;
}

void PQPlayer::TakePizza()
{
	_hasPizza = false;
}

void PQPlayer::SetInput(const vec2f &input)
{
	if(!_enabled) return;

	if(car)
	{
		car->input = input;
	}
	else
	{
		this->input = input;

		int spriteRow = -1;

		float angle = 0.0f;

		if(input.x != 0.0f || input.y != 0.0f)
		{
			angle = DEGREES(atan2(-input.y, input.x));

			if(angle < -angleTolerance)
				angle += 360.0f;

			for(int i = 0; i < nAngles; i++)
			{
				if(angle >= angles[i] - angleTolerance
				&& angle <= angles[i] + angleTolerance)
				{
					spriteRow = i;
					break;
				}
			}
		}

		shared_ptr<Animation> *anims = (_hasPizza ? animWithPizza : animNoPizza);
		image = (_hasPizza ? imgWithPizza : imgNoPizza).get();

		if(spriteRow >= 0)
		{
			if(animation != anims[spriteRow].get())
			{
				if(animation) animation->Stop();
				animation = anims[spriteRow].get();
				animation->Play(true, false);
			}
		}
		else
		{
			if(animation)
			{
				animation->Stop();
				animation = nullptr;
			}

			image->SetColumn(0);
		}
	}
}

void PQPlayer::ToggleInCar()
{
	if(car == nullptr)
		EnterCar();
	else
		ExitCar();
}

void PQPlayer::EnterCar()
{
	if(!_enabled) return;

	auto game = static_pointer_cast<PQGame>(state());

	float closestDist;
	PQVehicle *closestCar = nullptr;

	// find the closest car
	for(int i = 0; i < (int)game->cars.size(); i++)
	{
		float dist = (position - game->cars[i]->position).Length();

		if(dist > 120.f)
			continue;

		if(closestCar == nullptr || dist < closestDist)
		{
			closestDist = dist;
			closestCar = game->cars[i].get();
		}
	}

	// if any cars are close enough,
	if(closestCar != nullptr)
	{
		// disable player
		body->active(false);
		this->_visible = false;

		// get into the car and unfreeze it
		car = closestCar;
		car->input.set(0, 0);
		car->Unfreeze();

		RunCoroutine([this](yield_token<float> yield)
		{
			enterCar->Play();
			
			yield(0.1f);

			carStart->Play();

			float length = 0.9f;
			float start = Time::time();
			float finish = start + length;
			
			carIdle->Play();
			carIdle->SetGain(0);

			while(Time::time() < finish)
			{
				float elapsed = (Time::time() - start) / length;
				carIdle->SetGain(math::clamp(elapsed, 0.0f, _idleGain));

				yield(0);
			}

			carIdle->SetGain(_idleGain);

			_driveSoundGain = 0;
			carDrive->Play();
			carDrive->SetGain(_driveSoundGain);

			_carStarted = true;
		});
	}
	else
	{
		PizzaQuest::sounds().error->Play();
	}
}

void PQPlayer::ExitCar()
{
	if(!_enabled) return;

	auto game = static_pointer_cast<PQGame>(state());

	car->body->active(false);

	vec2f exitVector = car->body->left() * Physics::toMeters(50.0f);
	
	RayCast callback;
	game->physics->world()->RayCast(&callback,
									(b2Vec2&)car->body->position(),
									(b2Vec2&)(car->body->position() + exitVector));
	
	// can't get out
	if(callback.hits > 0)
	{
		car->body->active(true);
		PizzaQuest::sounds().error->Play();
		return;
	}

	this->body->active(true);
	this->body->transform(car->body->position() + exitVector, body->angle());
	
	b2Manifold manifold;
	b2CollidePolygonAndCircle(&manifold,
							  (b2PolygonShape*)car->body->b2_Body()->GetFixtureList()->GetShape(),
							  car->body->b2_Body()->GetTransform(),
							  (b2CircleShape*)body->b2_Body()->GetFixtureList()->GetShape(),
							  body->b2_Body()->GetTransform());

	// can't get out
	if(manifold.pointCount > 0)
	{
		car->body->active(true);
		this->body->active(false);
		PizzaQuest::sounds().error->Play();
		return;
	}
	
	this->position = body->pixelPosition();
	this->_visible = true;

	car->body->active(true);
	car->Freeze();
	car = nullptr;
	
	carIdle->Stop();
	carDrive->Stop();
	exitCar->Play();

	_carStarted = false;
}
