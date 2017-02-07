/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Object.h"

class Transform : public Object
{
	vec2f _pos;
	float _angle;
	float _scale;
	mutable bool _dirty;
	mutable mat3f _matrix;
	mutable float _cosAng;
	mutable float _sinAng;

	void _updateMatrix() const;

public:
	Transform();
	~Transform();

	vec2f position() const;
	void position(const vec2f &setPosition);

	float angle() const;
	void angle(float setAngle);

	float scale() const;
	void scale(float setScale);

	vec2f left() const;
	vec2f right() const;
	vec2f up() const;
	vec2f down() const;

	const mat3f &matrix() const;
};
