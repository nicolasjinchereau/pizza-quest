/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <list>
#include "Sprite.h"
#include "Trace.h"
#include "Camera.h"
#include "PQGameTypes.h"
#include "Object.h"
#include "PQDelivery.h"


class PQGame;

class PQCompass : public Object
{
private:
	shared_ptr<Sprite> compassBackground;
	shared_ptr<Sprite> greenArrow;
	shared_ptr<Sprite> redArrow;
	Sprite *arrow;

	vec2f _normalizedPosition;
	vec2f destination;
	float scale;
public:

	PQCompass();
	~PQCompass();

	// compass will point towards destination
	void SetDestination(const vec2f &dest);
	void ShowGreenArrow();
	void ShowRedArrow();
	void ShowNoArrow();

	// set screen position of compass
	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;

	virtual void Start() override;
	virtual void Draw() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);
};
