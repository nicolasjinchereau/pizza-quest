/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "PQCharacter.h"
#include "Sprite.h"
#include "Animation.h"
#include "Trace.h"
#include <Box2D.h>
#include "PQB2Shapes.h"
#include "PQVehicle.h"
#include "Time.h"
#include "Object.h"
#include "PQGameTypes.h"
#include "Sound.h"
#include "Camera.h"
#include "Transform.h"
#include "property.h"
#include "ParticleSystem.h"

class PQGame;
class PQVehicle;

class PQPlayer : public Object
{
	friend class PQVehicle;

	bool _hasPizza;
	bool _visible;
	bool _enabled;
	bool _carStarted;
	float _driveSoundGain;
	float _idleGain;

	int _health;
	vec2f input;
	Sprite *image;
	Animation *animation;

	shared_ptr<Sprite> imgWithPizza;
	shared_ptr<Sprite> imgNoPizza;
	shared_ptr<Animation> animWithPizza[8];
	shared_ptr<Animation> animNoPizza[8];
	shared_ptr<Sound> enterCar;
	shared_ptr<Sound> exitCar;
	shared_ptr<Sound> carStart;
	shared_ptr<Sound> carIdle;
	shared_ptr<Sound> carDrive;
	shared_ptr<Sound> nitrous;
	shared_ptr<ParticleSystem> exhaust;

	float angleTolerance;
	int nAngles;
	unique_ptr<float[]> angles;
public:

	PQPlayer(const vec2f &pos);
	~PQPlayer();
	
	PQVehicle *car;
	vec2f position;
	shared_ptr<RigidBody> body;

	virtual void Start() override;
	virtual void Update();
	virtual void LateUpdate();
	virtual void Draw() override;
	virtual string tag() const override { return "Player"; }

	bool IsAlive();
	vec2f GetPosition();

	void GivePizza();
	void TakePizza();
	bool HasPizza();

	void SetPosition(int x, int y);
	void SetInput(const vec2f &input);
	
	void ToggleInCar();
	void EnterCar();
	void ExitCar();

	property<int> health()
	{
		struct property {
			PQPlayer *object;

			int get() {
				return object->_health;
			}

			void set(int value) {
				object->_health = math::clamp(value, 0, 100);
			}
		};

		return property{this};
	}

	property<bool> enabled()
	{
		struct property {
			PQPlayer *object;

			bool get() {
				return object->_enabled;
			}

			void set(bool value) {
				if(!value)
					object->SetInput(vec2f::zero);

				if(object->car)
					object->car->body->active(value);
				else
					object->body->active(value);

				object->_enabled = value;
			}
		};

		return property{this};
	}
};
