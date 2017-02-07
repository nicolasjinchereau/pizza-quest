/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "State.h"
#include "Sprite.h"
#include <vector>
#include "Math.h"
#include "Engine.h"
#include <array>

using namespace std;

class PQCredits : public State
{
public:
	struct CreditEntry
	{
		CreditEntry(shared_ptr<Sprite> entry, float indent, float trailingSpace)
		{
			this->entry = entry;
			this->indent = indent;
			this->trailingSpace = trailingSpace;
		}

		vec2f pos;
		float indent;
		float trailingSpace;
		shared_ptr<Sprite> entry;
	};

	shared_ptr<Camera> camera;
	shared_ptr<Sprite> background;

	vector<CreditEntry> credits;
	float currentY;
	float currentX;
	float lastY;

	PQCredits();
	~PQCredits();

	virtual void Start() override;
	virtual void Update() override;

	virtual void OnBackPressed();
	virtual void OnTouchUp(float x, float y, int id);
	void AddCredit(shared_ptr<Texture> entry, float indent, float trailingSpace);
};
