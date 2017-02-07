/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <memory>
#include "Object.h"
#include "Sprite.h"


class PQNitroGauge : public Object
{
	shared_ptr<Sprite> gauge;
	shared_ptr<Sprite> needle;
	float _psi;
	vec2f _normPos;
	float _minAngle;
	float _maxAngle;
	bool _visible;
public:
	
	PQNitroGauge();
	~PQNitroGauge();

	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;

	float psi() const;
	void psi(float setPsi);

	bool visible() const;
	void visible(bool setVisible);

	virtual void Start() override;
	virtual void Draw() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);
};
