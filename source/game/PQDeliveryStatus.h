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


class PQDeliveryStatus : public Object
{
	shared_ptr<Texture> pizzaIconGray;
	shared_ptr<Texture> pizzaIconFull;
	shared_ptr<Texture> pizzaIconChecked;
	vector<shared_ptr<Sprite>> deliveries;

	float _scale;
	vec2f _normPos;
	bool _visible;
	uint32_t _completed;
	
public:
	PQDeliveryStatus();
	~PQDeliveryStatus();

	void normalizedPosition(const vec2f &setPosition);
	vec2f normalizedPosition() const;

	void deliveryCount(uint32_t setDeliveryCount);
	uint32_t deliveryCount() const;

	uint32_t deliveriesCompleted() const;
	
	vec2f deliveryPos(uint32_t index);

	void AddReadyPizza(const vec2f &pickedUpAtScreenPos);
	void AddCompletedDelivery(const vec2f &deliveredAtScreenPos);

	virtual void Draw() override;

	void GetLoadTasks(vector<function<void()>> &loadTasks);
};
