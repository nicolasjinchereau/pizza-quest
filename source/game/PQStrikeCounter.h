/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"
#include "Texture.h"
#include "Sprite.h"
#include "Math.h"
#include <vector>


class PQStrikeCounter : public Object
{
	shared_ptr<Texture> strikeTexture;
	shared_ptr<Sprite> strikeImage;
	shared_ptr<Sprite> strikeImageGray;
	shared_ptr<Sound> gavelStrike;

	float _scale;
	vec2f _normPos;
	bool _visible;
	uint32_t _strikes;

public:
	PQStrikeCounter();
	~PQStrikeCounter();

	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;

	void strikeCount(uint32_t setStrikeCount);
	uint32_t strikeCount() const;
	vec2f strikePos(uint32_t index);

	void AddStrike();

	virtual void Draw() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);
};
