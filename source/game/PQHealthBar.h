/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Sprite.h"
#include <string>
#include "property.h"

using namespace std;

class PQHealthBar : public Object
{
public:
	PQHealthBar();
	virtual ~PQHealthBar();

	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;
	
	property<float> fill()
	{
		struct property
		{
			PQHealthBar *object;

			float get() {
				return object->_fill;
			}

			void set(float value) {
				object->_fill = math::clamp(value, 0.0f, 1.0f);
			}
		};

		return property{this};
	}

	virtual void Start() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);

	void DoRedPulse();
	void DoBlackPulse();

private:
	shared_ptr<Shader> tintShader;
	shared_ptr<Shader> colorizeShader;
	shared_ptr<Sprite> heart;
	shared_ptr<Sprite> background;
	shared_ptr<Sprite> border;
	shared_ptr<Sprite> filler;
	weak_ptr<Task> pulseRoutine;

	float _scale;
	vec2f _normPos;
	bool _visible;
	float _fill;
	float _smoothFill;
	float _smoothSpeed;
};
