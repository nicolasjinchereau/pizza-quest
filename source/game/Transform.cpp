/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Transform.h"

Transform::Transform()
	: _pos(vec2f::zero),
	  _angle(0),
	  _scale(0)
{
	_dirty = true;
}

Transform::~Transform()
{

}

vec2f Transform::position() const
{
	return _pos;
}

void Transform::position(const vec2f &setPosition)
{
	_pos = setPosition;
	_dirty = true;
}

float Transform::angle() const
{
	return _angle;
}

void Transform::angle(float setAngle)
{
	_angle = setAngle;
	_dirty = true;
}

float Transform::scale() const
{
	return _scale;
}

void Transform::scale(float setScale)
{
	_scale = setScale;
	_dirty = true;
}

vec2f Transform::left() const
{
	_updateMatrix();
	return vec2f(-_cosAng, -_sinAng);
}

vec2f Transform::right() const
{
	_updateMatrix();
	return vec2f(_cosAng, _sinAng);
}

vec2f Transform::up() const
{
	_updateMatrix();
	return vec2f(-_sinAng, _cosAng);
}

vec2f Transform::down() const
{
	_updateMatrix();
	return vec2f(_sinAng, -_cosAng);
}

const mat3f &Transform::matrix() const
{
	_updateMatrix();
	return _matrix;
}

void Transform::_updateMatrix() const
{
	if(_dirty)
	{
		_cosAng = cos(_angle);
		_sinAng = sin(_angle);

		_matrix.Set(_cosAng * _scale, -_sinAng * _scale, _pos.x,
					_sinAng * _scale, _cosAng * _scale, _pos.y,
					_scale, _scale, 1);

		_dirty = false;
	}
}
