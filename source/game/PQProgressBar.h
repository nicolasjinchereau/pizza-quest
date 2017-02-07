/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <mutex>
#include "Sprite.h"
#include "Math.h"
#include "Camera.h"
#include <string>
#include "State.h"


using namespace std;

class PQProgressBar : public Object
{
public:
	PQProgressBar();
	virtual ~PQProgressBar();

	bool OpenImages(const string &fnBackground, const string &fnBorder, const string &fnFiller);

	void SetProgress(float progress);
	void AddProgress(float progress);
	float GetProgress();

	void SetPosition(float wx, float wy);

	virtual void Start() override;

private:
	vec2f position;
	shared_ptr<Sprite> background;
	shared_ptr<Sprite> border;
	shared_ptr<Sprite> filler;
	
	float progress;
};
