/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Math.h"

///////////////////////////////////////////////////////////////////////////////
//    Rect
///////////////////////////////////////////////////////////////////////////////

void Rect::Set(float X, float Y, float W, float H)
{
	x = X;
	y = Y;
	w = W;
	h = H;
}

void Rect::Offset(float offset_x, float offset_y)
{
	x += offset_x;
	y += offset_y;
}

void Rect::Deflate(float px, float py)
{
	x += px;
	y += py;
	w -= 2*px;
	h -= 2*py;
}

bool Rect::Intersects(const Rect &rect) const
{
	return !(x > rect.x + rect.w
			|| x + w < rect.x
			|| y > rect.y + rect.h
			|| y + h < rect.y);
}

bool Rect::PointInside(float X, float Y) const
{
	return !(X < x
			|| X > x + w - 1
			|| Y < y
			|| Y > y + h - 1);
}



///////////////////////////////////////////////////////////////////////////////
//    mat3f
///////////////////////////////////////////////////////////////////////////////

void mat3f::Set(float _11, float _21, float _31,
					float _12, float _22, float _32,
					float _13, float _23, float _33)
{
	m11 = _11; m21 = _21; m31 = _31;
	m12 = _12; m22 = _22; m32 = _32;
	m13 = _13; m23 = _23; m33 = _33;
}

vec2f mat3f::operator*(const vec2f &V) const
{
	return vec2f(m11 * V.x + m21 * V.y + m31,
					m12 * V.x + m22 * V.y + m32);
}

vec3f mat3f::operator*(const vec3f &V) const
{
	return vec3f(m11 * V.x + m21 * V.y + m31 * V.z,
					m12 * V.x + m22 * V.y + m32 * V.z,
					m13 * V.x + m23 * V.y + m33 * V.z);
}

mat3f mat3f::operator*(const mat3f &M) const
{
	return mat3f(m11 * M.m11 + m21 * M.m12 + m31 * M.m13,
				m11 * M.m21 + m21 * M.m22 + m31 * M.m23,
				m11 * M.m31 + m21 * M.m32 + m31 * M.m33,

				m12 * M.m11 + m22 * M.m12 + m32 * M.m13,
				m12 * M.m21 + m22 * M.m22 + m32 * M.m23,
				m12 * M.m31 + m22 * M.m32 + m32 * M.m33,
					
				m13 * M.m11 + m23 * M.m12 + m33 * M.m13,
				m13 * M.m21 + m23 * M.m22 + m33 * M.m23,
				m13 * M.m31 + m23 * M.m32 + m33 * M.m33);
}

mat3f &mat3f::operator*=(const mat3f &M)
{
	*this = mat3f(m11 * M.m11 + m21 * M.m12 + m31 * M.m13,
					m11 * M.m21 + m21 * M.m22 + m31 * M.m23,
					m11 * M.m31 + m21 * M.m32 + m31 * M.m33,

					m12 * M.m11 + m22 * M.m12 + m32 * M.m13,
					m12 * M.m21 + m22 * M.m22 + m32 * M.m23,
					m12 * M.m31 + m22 * M.m32 + m32 * M.m33,
					
					m13 * M.m11 + m23 * M.m12 + m33 * M.m13,
					m13 * M.m21 + m23 * M.m22 + m33 * M.m23,
					m13 * M.m31 + m23 * M.m32 + m33 * M.m33);

	return *this;
}

void mat3f::Translate(float x, float y)
{
	*this = mat3f::Translation(x, y) * (*this);
}

void mat3f::SetOrtho2D(float left, float right, float bottom, float top)
{
	float dx = right - left;
	float dy = top - bottom;

	float tx;
	float ty;
	float sx;
	float sy;

	if(dx != 0)
	{
		tx = -(right + left) / dx;
		sx = 2.0f / dx;
	}
	else
	{
		tx = 0;
		sx = 0;
	}

	if(dy != 0)
	{
		ty = -(top + bottom) / dy;
		sy = 2.0f / dy;
	}
	else
	{
		ty = 0;
		sy = 0;
	}

	Set(sx, 0, tx,
		0, sy, ty,
		0, 0, 1);
}

void mat3f::SetIdentity()
{
	Set(1, 0, 0,
		0, 1, 0,
		0, 0, 1);
}

void mat3f::SetTranslation(float x, float y)
{
	Set(1, 0, x,
		0, 1, y,
		0, 0, 1);
}

void mat3f::SetScale(float sx, float sy)
{
	Set(sx, 0, 0,
		0, sy, 0,
		0, 0, 1);
}

void mat3f::SetRotation(float angle)
{
	float radAng = RADIANS(angle);
	float cf = cosf(radAng);
	float sf = sinf(radAng);

	Set(cf,	-sf, 0,
		sf, cf,  0,
		0,	0,   1);
}

void mat3f::Transpose()
{
	Set(m11, m12, m13,
		m21, m22, m23,
		m31, m32, m33);
}

mat3f mat3f::Scale(float sx, float sy, float sw)
{
	return mat3f(sx, 0,  0,
				0,  sy, 0,
				0,  0,  sw);
}

mat3f mat3f::Translation(float x, float y)
{
	return mat3f(1, 0, x,
				0, 1, y,
				0, 0, 1);
}

mat3f mat3f::Identity()
{
	return mat3f(1, 0, 0,
				0, 1, 0,
				0, 0, 1);
}

mat3f mat3f::Ortho2D(float left, float right, float bottom, float top)
{
	float dx = right - left;
	float dy = top - bottom;

	float tx;
	float ty;
	float sx;
	float sy;

	if(dx != 0)
	{
		tx = -(right + left) / dx;
		sx = 2.0f / dx;
	}
	else
	{
		tx = 0;
		sx = 0;
	}

	if(dy != 0)
	{
		ty = -(top + bottom) / dy;
		sy = 2.0f / dy;
	}
	else
	{
		ty = 0;
		sy = 0;
	}

	return mat3f(sx, 0, tx,
				0, sy, ty,
				0, 0, 1);
}

///////////////////////////////////////////////////////////////////////////////
//    mat4f
///////////////////////////////////////////////////////////////////////////////

void mat4f::Set(float _11, float _21, float _31, float _41,
			float _12, float _22, float _32, float _42,
			float _13, float _23, float _33, float _43,
			float _14, float _24, float _34, float _44)
{
	m11 = _11; m21 = _21; m31 = _31; m41 = _41;
	m12 = _12; m22 = _22; m32 = _32; m42 = _42;
	m13 = _13; m23 = _23; m33 = _33; m43 = _43;
	m14 = _14; m24 = _24; m34 = _34; m44 = _44;
}

vec4f mat4f::operator*(const vec4f &V) const
{
	return vec4f(m11 * V.x + m21 * V.y + m31 * V.z + m41 * V.w,
				 m12 * V.x + m22 * V.y + m32 * V.z + m42 * V.w,
				 m13 * V.x + m23 * V.y + m33 * V.z + m43 * V.w,
				 m14 * V.x + m24 * V.y + m34 * V.z + m44 * V.w);
}

mat4f mat4f::operator*(const mat4f &M) const
{
	return mat4f(m11 * M.m11 + m21 * M.m12 + m31 * M.m13 + m41 * M.m14,
				m11 * M.m21 + m21 * M.m22 + m31 * M.m23 + m41 * M.m24,
				m11 * M.m31 + m21 * M.m32 + m31 * M.m33 + m41 * M.m34,
				m11 * M.m41 + m21 * M.m42 + m31 * M.m43 + m41 * M.m44,

				m12 * M.m11 + m22 * M.m12 + m32 * M.m13 + m42 * M.m14,
				m12 * M.m21 + m22 * M.m22 + m32 * M.m23 + m42 * M.m24,
				m12 * M.m31 + m22 * M.m32 + m32 * M.m33 + m42 * M.m34,
				m12 * M.m41 + m22 * M.m42 + m32 * M.m43 + m42 * M.m44,

				m13 * M.m11 + m23 * M.m12 + m33 * M.m13 + m43 * M.m14,
				m13 * M.m21 + m23 * M.m22 + m33 * M.m23 + m43 * M.m24,
				m13 * M.m31 + m23 * M.m32 + m33 * M.m33 + m43 * M.m34,
				m13 * M.m41 + m23 * M.m42 + m33 * M.m43 + m43 * M.m44,

				m14 * M.m11 + m24 * M.m12 + m34 * M.m13 + m44 * M.m14,
				m14 * M.m21 + m24 * M.m22 + m34 * M.m23 + m44 * M.m24,
				m14 * M.m31 + m24 * M.m32 + m34 * M.m33 + m44 * M.m34,
				m14 * M.m41 + m24 * M.m42 + m34 * M.m43 + m44 * M.m44);
}

mat4f mat4f::operator*(float s) const
{
	return mat4f(m11 * s, m21 * s, m31 * s, m41 * s,
				 m12 * s, m22 * s, m32 * s, m42 * s,
				 m13 * s, m23 * s, m33 * s, m43 * s,
				 m14 * s, m24 * s, m34 * s, m44 * s);
}

mat4f mat4f::operator/(float s) const
{
	return mat4f(m11 / s, m21 / s, m31 / s, m41 / s,
				 m12 / s, m22 / s, m32 / s, m42 / s,
				 m13 / s, m23 / s, m33 / s, m43 / s,
				 m14 / s, m24 / s, m34 / s, m44 / s);
}

mat4f &mat4f::operator*=(const mat4f &M)
{
	*this = mat4f(m11 * M.m11 + m21 * M.m12 + m31 * M.m13 + m41 * M.m14,
				m11 * M.m21 + m21 * M.m22 + m31 * M.m23 + m41 * M.m24,
				m11 * M.m31 + m21 * M.m32 + m31 * M.m33 + m41 * M.m34,
				m11 * M.m41 + m21 * M.m42 + m31 * M.m43 + m41 * M.m44,

				m12 * M.m11 + m22 * M.m12 + m32 * M.m13 + m42 * M.m14,
				m12 * M.m21 + m22 * M.m22 + m32 * M.m23 + m42 * M.m24,
				m12 * M.m31 + m22 * M.m32 + m32 * M.m33 + m42 * M.m34,
				m12 * M.m41 + m22 * M.m42 + m32 * M.m43 + m42 * M.m44,

				m13 * M.m11 + m23 * M.m12 + m33 * M.m13 + m43 * M.m14,
				m13 * M.m21 + m23 * M.m22 + m33 * M.m23 + m43 * M.m24,
				m13 * M.m31 + m23 * M.m32 + m33 * M.m33 + m43 * M.m34,
				m13 * M.m41 + m23 * M.m42 + m33 * M.m43 + m43 * M.m44,

				m14 * M.m11 + m24 * M.m12 + m34 * M.m13 + m44 * M.m14,
				m14 * M.m21 + m24 * M.m22 + m34 * M.m23 + m44 * M.m24,
				m14 * M.m31 + m24 * M.m32 + m34 * M.m33 + m44 * M.m34,
				m14 * M.m41 + m24 * M.m42 + m34 * M.m43 + m44 * M.m44);
	return *this;
}

mat4f &mat4f::operator*=(float s)
{
	m11 *= s;	m21 *= s;	m31 *= s;	m41 *= s;
	m12 *= s;	m22 *= s;	m32 *= s;	m42 *= s;
	m13 *= s;	m23 *= s;	m33 *= s;	m43 *= s;
	m14 *= s;	m24 *= s;	m34 *= s;	m44 *= s;
	return *this;
}

mat4f &mat4f::operator/=(float s)
{
	m11 /= s;	m21 /= s;	m31 /= s;	m41 /= s;
	m12 /= s;	m22 /= s;	m32 /= s;	m42 /= s;
	m13 /= s;	m23 /= s;	m33 /= s;	m43 /= s;
	m14 /= s;	m24 /= s;	m34 /= s;	m44 /= s;
	return *this;
}

float mat4f::operator[](int i) const
{
	return ((float*)this)[i];
}

void mat4f::Translate(float x, float y, float z)
{
	*this = mat4f::Translation(x, y, z) * (*this);
}

void mat4f::SetOrtho2D(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = zFar - zNear;

	float tx = (dx != 0) ? -(right + left) / dx : 0;
	float ty = (dy != 0) ? -(top + bottom) / dy : 0;
	float tz = (dz != 0) ? -(zFar + zNear) / dz : 0;

	Set(2.0f / dx, 0,			0,			tx,
		0,			2.0f / dy, 0,			ty,
		0,			0,			-2.0f / dz, tz,
		0,			0,			0,			1);
}

void mat4f::SetProject3D(float fov, float view_w, float view_h, float ZNear, float ZFar)
{
	float Delta = ZNear - ZFar;
	float aspect = view_h / view_w;

	float f = 1.0f / tanf(RADIANS(fov / 2.0f));
	
	float sz = (ZFar + ZNear) / Delta;

	Set(f,			0,			0,			0,
		0,			f / aspect,	0,			0,
		0,			0,			sz,			-2.0f * ZFar * ZNear / Delta,
		0,			0,			-1.0,		0);
}

void mat4f::SetLookAt(const vec3f &eye, const vec3f &center, const vec3f &up)
{
// forward
	vec3f forward(center.x - eye.x,
				  center.y - eye.y,
				  center.z - eye.z);

	forward.Normalize();
		
// side
	vec3f side = forward.Cross(up);

	side.Normalize();
		
// up
	vec3f newUp = side.Cross(forward);
		
// result
	Set(side.x, 		side.y,		side.z,		0,
		newUp.x,		newUp.y,	newUp.z,	0,
		-forward.x,		-forward.y,	-forward.z,	0,
		0,				0,			0,			1);

// translate to eye
	Translate(-eye.x, -eye.y, -eye.z);
}

void mat4f::SetIdentity()
{
	Set(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

void mat4f::SetTranslation(float x, float y, float z)
{
	Set(1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1);
}

void mat4f::SetScale(float sx, float sy, float sz, float sw)
{
	Set(sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, sz, 0,
		0, 0, 0, sw);
}

void mat4f::SetRotationX(float angle)
{
	float radAng = RADIANS(angle);
	float cf = cosf(radAng);
	float sf = sinf(radAng);
	Set(1, 0, 0, 0,
		0, cf, -sf, 0,
		0, sf, cf, 0,
		0, 0, 0, 1);
}

void mat4f::SetRotationY(float angle)
{
	float radAng = RADIANS(angle);
	float cf = cosf(radAng);
	float sf = sinf(radAng);
	Set(cf, 0, sf, 0,
		0, 1, 0, 0,
		-sf, 0, cf, 0,
		0, 0, 0, 1);
}

void mat4f::SetRotationZ(float angle)
{
	float radAng = RADIANS(angle);
	float cf = cosf(radAng);
	float sf = sinf(radAng);
	Set(cf, -sf, 0, 0,
		sf, cf, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

void mat4f::Transpose()
{
	Set(m11, m12, m13, m14,
		m21, m22, m23, m24,
		m31, m32, m33, m34,
		m41, m42, m43, m44);
}

bool mat4f::Invert()
{
	mat4f tmp;

	tmp.m11 = m22 * m33 * m44 - m22 * m34 * m43 - m32 * m23 * m44 + m32 * m24 * m43 + m42 * m23 * m34 - m42 * m24 * m33;
	tmp.m21 = -m21 * m33 * m44 + m21 * m34 * m43 + m31 * m23 * m44 - m31 * m24 * m43 - m41 * m23 * m34 + m41 * m24 * m33;
	tmp.m31 = m21  *  m32 * m44 - m21 * m34 * m42 - m31 * m22 * m44 + m31 * m24 * m42 + m41 * m22 * m34 - m41 * m24 * m32;
	tmp.m41 = -m21 * m32 * m43 + m21 * m33 * m42 + m31 * m22 * m43 - m31 * m23 * m42 - m41 * m22 * m33 + m41 * m23 * m32;
	tmp.m12 = -m12 * m33 * m44 + m12 * m34 * m43 + m32 * m13 * m44 - m32 * m14 * m43 - m42 * m13 * m34 + m42 * m14 * m33;
	tmp.m22 = m11 * m33 * m44 - m11 * m34 * m43 - m31 * m13 * m44 + m31 * m14 * m43 + m41 * m13 * m34 - m41 * m14 * m33;
	tmp.m32 = -m11 * m32 * m44 + m11 * m34 * m42 + m31 * m12 * m44 - m31 * m14 * m42 - m41 * m12 * m34 + m41 * m14 * m32;
	tmp.m42 = m11 * m32 * m43 - m11 * m33 * m42 - m31 * m12 * m43 + m31 * m13 * m42 + m41 * m12 * m33 - m41 * m13 * m32;
	tmp.m13 = m12 * m23 * m44 - m12 * m24 * m43 - m22 * m13 * m44 + m22 * m14 * m43 + m42 * m13 * m24 - m42 * m14 * m23;
	tmp.m23 = -m11 * m23 * m44 + m11 * m24 * m43 + m21 * m13 * m44 - m21 * m14 * m43 - m41 * m13 * m24 + m41 * m14 * m23;
	tmp.m33 = m11 * m22 * m44 - m11 * m24 * m42 - m21 * m12 * m44 + m21 * m14 * m42 + m41 * m12 * m24 - m41 * m14 * m22;
	tmp.m43 = -m11 * m22 * m43 + m11 * m23 * m42 + m21 * m12 * m43 - m21 * m13 * m42 - m41 * m12 * m23 + m41 * m13 * m22;
	tmp.m14 = -m12 * m23 * m34 + m12 * m24 * m33 + m22 * m13 * m34 - m22 * m14 * m33 - m32 * m13 * m24 + m32 * m14 * m23;
	tmp.m24 = m11 * m23 * m34 - m11 * m24 * m33 - m21 * m13 * m34 + m21 * m14 * m33 + m31 * m13 * m24 - m31 * m14 * m23;
	tmp.m34 = -m11 * m22 * m34 + m11 * m24 * m32 + m21 * m12 * m34 - m21 * m14 * m32 - m31 * m12 * m24 + m31 * m14 * m22;
	tmp.m44 = m11 * m22 * m33 - m11 * m23 * m32 - m21 * m12 * m33 + m21 * m13 * m32 + m31 * m12 * m23 - m31 * m13 * m22;

	float det = m11 * tmp.m11 + m12 * tmp.m21 + m13 * tmp.m31 + m14 * tmp.m41;

	if(det == 0.0f)
		return false;

	*this = tmp / det;

	return true;
}

mat4f mat4f::Inverse()
{
	mat4f tmp;

	tmp.m11 = m22 * m33 * m44 - m22 * m34 * m43 - m32 * m23 * m44 + m32 * m24 * m43 + m42 * m23 * m34 - m42 * m24 * m33;
	tmp.m21 = -m21 * m33 * m44 + m21 * m34 * m43 + m31 * m23 * m44 - m31 * m24 * m43 - m41 * m23 * m34 + m41 * m24 * m33;
	tmp.m31 = m21  *  m32 * m44 - m21 * m34 * m42 - m31 * m22 * m44 + m31 * m24 * m42 + m41 * m22 * m34 - m41 * m24 * m32;
	tmp.m41 = -m21 * m32 * m43 + m21 * m33 * m42 + m31 * m22 * m43 - m31 * m23 * m42 - m41 * m22 * m33 + m41 * m23 * m32;
	tmp.m12 = -m12 * m33 * m44 + m12 * m34 * m43 + m32 * m13 * m44 - m32 * m14 * m43 - m42 * m13 * m34 + m42 * m14 * m33;
	tmp.m22 = m11 * m33 * m44 - m11 * m34 * m43 - m31 * m13 * m44 + m31 * m14 * m43 + m41 * m13 * m34 - m41 * m14 * m33;
	tmp.m32 = -m11 * m32 * m44 + m11 * m34 * m42 + m31 * m12 * m44 - m31 * m14 * m42 - m41 * m12 * m34 + m41 * m14 * m32;
	tmp.m42 = m11 * m32 * m43 - m11 * m33 * m42 - m31 * m12 * m43 + m31 * m13 * m42 + m41 * m12 * m33 - m41 * m13 * m32;
	tmp.m13 = m12 * m23 * m44 - m12 * m24 * m43 - m22 * m13 * m44 + m22 * m14 * m43 + m42 * m13 * m24 - m42 * m14 * m23;
	tmp.m23 = -m11 * m23 * m44 + m11 * m24 * m43 + m21 * m13 * m44 - m21 * m14 * m43 - m41 * m13 * m24 + m41 * m14 * m23;
	tmp.m33 = m11 * m22 * m44 - m11 * m24 * m42 - m21 * m12 * m44 + m21 * m14 * m42 + m41 * m12 * m24 - m41 * m14 * m22;
	tmp.m43 = -m11 * m22 * m43 + m11 * m23 * m42 + m21 * m12 * m43 - m21 * m13 * m42 - m41 * m12 * m23 + m41 * m13 * m22;
	tmp.m14 = -m12 * m23 * m34 + m12 * m24 * m33 + m22 * m13 * m34 - m22 * m14 * m33 - m32 * m13 * m24 + m32 * m14 * m23;
	tmp.m24 = m11 * m23 * m34 - m11 * m24 * m33 - m21 * m13 * m34 + m21 * m14 * m33 + m31 * m13 * m24 - m31 * m14 * m23;
	tmp.m34 = -m11 * m22 * m34 + m11 * m24 * m32 + m21 * m12 * m34 - m21 * m14 * m32 - m31 * m12 * m24 + m31 * m14 * m22;
	tmp.m44 = m11 * m22 * m33 - m11 * m23 * m32 - m21 * m12 * m33 + m21 * m13 * m32 + m31 * m12 * m23 - m31 * m13 * m22;

	float det = m11 * tmp.m11 + m12 * tmp.m21 + m13 * tmp.m31 + m14 * tmp.m41;

	if(det == 0.0f)
		return *this;

	tmp /= det;

	return tmp;
}

mat4f mat4f::Scale(float sx, float sy, float sz, float sw)
{
	return mat4f(sx, 0, 0, 0,
				 0, sy, 0, 0,
				 0, 0, sz, 0,
				 0, 0, 0, sw);
}

mat4f mat4f::Translation(float x, float y, float z)
{
	return mat4f(1, 0, 0, x,
				 0, 1, 0, y,
				 0, 0, 1, z,
				 0, 0, 0, 1);
}

mat4f mat4f::Identity()
{
	return mat4f(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
}

mat4f mat4f::Ortho2D(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = zFar - zNear;

	float tx = (dx != 0) ? -(right + left) / dx : 0;
	float ty = (dy != 0) ? -(top + bottom) / dy : 0;
	float tz = (dz != 0) ? -(zFar + zNear) / dz : 0;

	return mat4f(2.0f / dx, 0,			0,			tx,
				0,			2.0f / dy, 0,			ty,
				0,			0,			-2.0f / dz, tz,
				0,			0,			0,			 1);
}

mat4f mat4f::Project3D(float fov, float view_w, float view_h, float ZNear, float ZFar)
{
	float Delta = ZNear - ZFar;
	float aspect = view_h / view_w;

	float f = 1.0f / tanf(RADIANS(fov / 2.0f));
	
	float sz = (ZFar + ZNear) / Delta;

	return(f,	0,			0,			0,
		   0,	f / aspect,	0,			0,
		   0,	0,			sz,			-2.0f * ZFar * ZNear / Delta,
		   0,	0,			-1.0,		0);
}

mat4f mat4f::LookAt(const vec3f &eye, const vec3f &center, const vec3f &up)
{
	// forward
	vec3f forward(center.x - eye.x,
				  center.y - eye.y,
				  center.z - eye.z);

	forward.Normalize();
		
	// side
	vec3f side = forward.Cross(up);
	side.Normalize();
		
	// up
	vec3f newUp = side.Cross(forward);
		
	// result
	mat4f result(side.x, 		side.y,		side.z,		0,
				newUp.x,		newUp.y,	newUp.z,	0,
				-forward.x,		-forward.y,	-forward.z,	0,
				0,				0,			0,			1);

	// translate to eye
	result.Translate(-eye.x, -eye.y, -eye.z);
		

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//    vec2f
///////////////////////////////////////////////////////////////////////////////

const vec2f vec2f::up(0, 1);
const vec2f vec2f::down(0, -1);
const vec2f vec2f::left(-1, 0);
const vec2f vec2f::right(1, 0);
const vec2f vec2f::zero(0, 0);
const vec2f vec2f::one(1, 1);

vec2f::vec2f(const mat3f &mtx, const vec2f &vec)
{
	x = mtx.m11 * vec.x + mtx.m21 * vec.y + mtx.m31;
	y = mtx.m12 * vec.x + mtx.m22 * vec.y + mtx.m32;
}

vec2f::vec2f(const mat3f &mtx, float x, float y)
{
	this->x = mtx.m11 * x + mtx.m21 * y + mtx.m31;
	this->y = mtx.m12 * x + mtx.m22 * y + mtx.m32;
}

vec2f vec2f::operator+(const vec2f &vec) const
{
	return vec2f(x + vec.x, y + vec.y);
}
vec2f vec2f::operator-(const vec2f &vec) const
{
	return vec2f(x - vec.x, y - vec.y);
}
vec2f vec2f::operator-() const
{
	return vec2f(-x, -y);
}
vec2f vec2f::operator*(const float scalar) const
{
	return vec2f(x * scalar, y * scalar);
}
float vec2f::operator*(const vec2f &vec) const
{
	return x * vec.x + y * vec.y;
}
vec2f vec2f::operator/(const float scalar) const
{
	return vec2f(x / scalar, y / scalar);
}
vec2f &vec2f::operator+=(const vec2f &vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}
vec2f &vec2f::operator-=(const vec2f &vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}
vec2f &vec2f::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}
vec2f vec2f::operator*(const mat3f &M) const
{
	return vec2f(x * M.m11 + y * M.m12 + M.m13,
				x * M.m21 + y * M.m22 + M.m23);
}
vec2f &vec2f::operator*=(const mat3f &M)
{
	return *this = vec2f(x * M.m11 + y * M.m12 + M.m13,
						x * M.m21 + y * M.m22 + M.m23);
}
vec2f vec2f::operator*(const mat4f &M) const
{
	return vec2f(x * M.m11 + y * M.m12 + M.m14,
				x * M.m21 + y * M.m22 + M.m24);
}
vec2f &vec2f::operator*=(const mat4f &M)
{
	return *this = vec2f(x * M.m11 + y * M.m12 + M.m14,
						x * M.m21 + y * M.m22 + M.m24);
}
void vec2f::LeftTransform(const mat3f &M, vec2f &V)
{
	x = M.m11 * V.x + M.m21 * V.y + M.m31;
	y = M.m12 * V.x + M.m22 * V.y + M.m32;
}
void vec2f::LeftTransform(const mat4f &M, vec2f &V)
{
	x = M.m11 * V.x + M.m21 * V.y + M.m41;
	y = M.m12 * V.x + M.m22 * V.y + M.m42;
}
vec2f &vec2f::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}
bool vec2f::operator==(const vec2f &vec) const
{
	return (x == vec.x && y == vec.y);
}
bool vec2f::operator!=(const vec2f &vec) const
{
	return (x != vec.x || y != vec.y);
}
	
vec2f vec2f::PerpCW() const
{
	return vec2f(-y, x);
}

vec2f vec2f::PerpCCW() const
{
	return vec2f(y, -x);
}

float vec2f::Distance(const vec2f &vec) const
{
	vec2f dvec(vec.x - x, vec.y - y);
	return sqrt((dvec.x * dvec.x) + (dvec.y * dvec.y));
}
float vec2f::DistanceSq(const vec2f &vec) const
{
	vec2f dvec(vec.x - x, vec.y - y);
	return (dvec.x * dvec.x) + (dvec.y * dvec.y);
}
float vec2f::Length() const
{
	return sqrtf((x * x) + (y * y));
}
float vec2f::LengthSq() const
{
	return (x * x) + (y * y);
}
float vec2f::Cross(const vec2f &vec) const
{
	return x * vec.y - vec.x * y;
}

float vec2f::Dot(const vec2f &other) const
{
	return x * other.x + y * other.y;
}

// normalize vector , doesn't return or save the length
void vec2f::Normalize()
{
	float length = sqrtf(x * x + y * y);
	if(length > 0)
	{
		x /= length;
		y /= length;
	}
}

vec2f vec2f::Normalized() const
{
	float len = sqrt(x * x + y * y);
	return len > 0 ? vec2f(x / len, y / len) : vec2f::zero;
}

void vec2f::Reverse()
{
	x = -x;
	y = -y;
}

float vec2f::Angle(const vec2f &other) const
{
	return acos(Normalized().Dot(other.Normalized()));
}

vec2f vec2f::Project(const vec2f &v) const
{
	return v * this->Dot(v) / v.Dot(v);
}

vec2f vec2f::SignedProject(const vec2f &v) const
{
	float d = this->Dot(v);
	return d > 0 ? v * d / v.Dot(v) : vec2f::zero;
}

///////////////////////////////////////////////////////////////////////////////
//    vec3f
///////////////////////////////////////////////////////////////////////////////

const vec3f vec3f::zero(0, 0, 0);
const vec3f vec3f::forward(0, 0, -1);
const vec3f vec3f::up(0, 1, 0);
const vec3f vec3f::right(1, 0, 0);

vec3f vec3f::operator+(const vec3f &vec) const
{
	return vec3f(x + vec.x, y + vec.y, z + vec.z);
}
vec3f vec3f::operator-(const vec3f &vec) const
{
	return vec3f(x - vec.x, y - vec.y, z - vec.z);
}
vec3f vec3f::operator-() const
{
	return vec3f(-x, -y, -z);
}
vec3f vec3f::operator*(const float scalar) const
{
	return vec3f(x * scalar, y * scalar, z * scalar);
}
float vec3f::operator*(const vec3f &vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}
vec3f vec3f::operator*(const mat4f &M) const
{
	return vec3f( x * M.m11 + y * M.m12 + z * M.m13 + M.m14,
					x * M.m21 + y * M.m22 + z * M.m23 + M.m24,
					x * M.m31 + y * M.m32 + z * M.m33 + M.m34);
}
vec3f &vec3f::operator*=(const mat4f &M)
{
	return *this = vec3f( x * M.m11 + y * M.m12 + z * M.m13 + M.m14,
							x * M.m21 + y * M.m22 + z * M.m23 + M.m24,
							x * M.m31 + y * M.m32 + z * M.m33 + M.m34);
}
vec3f vec3f::operator/(const float scalar) const
{
	return vec3f(x / scalar, y / scalar, z / scalar);
}
vec3f &vec3f::operator+=(const vec3f &vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}
vec3f &vec3f::operator-=(const vec3f &vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}
vec3f &vec3f::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}
vec3f &vec3f::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}
bool vec3f::operator==(const vec3f &vec) const
{
	return (x == vec.x && y == vec.y && z == vec.z);
}
bool vec3f::operator!=(const vec3f &vec) const
{
	return (x != vec.x || y != vec.y || z != vec.z);
}
float vec3f::Dot(const vec3f &vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}
vec3f vec3f::Cross(const vec3f &vec) const
{
	return vec3f((y * vec.z) - (z * vec.y),
					(z * vec.x) - (x * vec.z),
					(x * vec.y) - (y * vec.x));
}
float vec3f::Distance(const vec3f &vec) const
{
	vec3f dvec(vec.x - x, vec.y - y, vec.z - z);
	return sqrtf(dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z);
}
float vec3f::DistanceSq(const vec3f &vec) const
{
	vec3f dvec(vec.x - x, vec.y - y, vec.z - z);
	return dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z;
}
float vec3f::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}
float vec3f::LengthSq() const
{
	return x * x + y * y + z * z;
}
vec3f &vec3f::Normalize()
{
	float length = sqrtf(x * x + y * y + z * z);
	if(length > 0)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	return *this;
}
void vec3f::Reverse()
{
	x = -x;
	y = -y;
	z = -z;
}

///////////////////////////////////////////////////////////////////////////////
//    vec4f
///////////////////////////////////////////////////////////////////////////////

const vec4f vec4f::zero(0, 0, 0);
const vec4f vec4f::forward(0, 0, -1);
const vec4f vec4f::up(0, 1, 0);
const vec4f vec4f::right(1, 0, 0);

vec4f vec4f::operator+(const vec4f &vec) const
{
	return vec4f(x + vec.x, y + vec.y, z + vec.z);
}
vec4f vec4f::operator-(const vec4f &vec) const
{
	return vec4f(x - vec.x, y - vec.y, z - vec.z);
}
vec4f vec4f::operator*(const float scalar) const
{
	return vec4f(x * scalar, y * scalar, z * scalar);
}
float vec4f::operator*(const vec4f &vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}
vec4f vec4f::operator*(const mat4f &M) const
{
	return vec4f(x * M.m11 + y * M.m21 + z * M.m31 + w * M.m41,
					x * M.m12 + y * M.m22 + z * M.m32 + w * M.m42,
					x * M.m13 + y * M.m23 + z * M.m33 + w * M.m43,
					x * M.m14 + y * M.m24 + z * M.m34 + w * M.m44);
}
vec4f &vec4f::operator*=(const mat4f &M)
{
	return *this = vec4f(x * M.m11 + y * M.m21 + z * M.m31 + w * M.m41,
							x * M.m12 + y * M.m22 + z * M.m32 + w * M.m42,
							x * M.m13 + y * M.m23 + z * M.m33 + w * M.m43,
							x * M.m14 + y * M.m24 + z * M.m34 + w * M.m44);
}
void vec4f::LeftTransform(const mat4f &M, const vec4f &V)
{
	x = M.m11 * V.x + M.m21 * V.y + M.m31 * V.z + M.m31 * V.w + M.m41;
	y = M.m12 * V.x + M.m22 * V.y + M.m32 * V.z + M.m32 * V.w + M.m42;
	z = M.m13 * V.x + M.m23 * V.y + M.m33 * V.z + M.m33 * V.w + M.m43;
	w = M.m14 * V.x + M.m24 * V.y + M.m34 * V.z + M.m34 * V.w + M.m44;
}
vec4f vec4f::operator/(const float scalar) const
{
	return vec4f(x / scalar, y / scalar, z / scalar);
}
vec4f &vec4f::operator+=(const vec4f &vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}
vec4f &vec4f::operator-=(const vec4f &vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}
vec4f &vec4f::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}
vec4f &vec4f::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}
bool vec4f::operator==(const vec4f &vec) const
{
	return (x == vec.x && y == vec.y && z == vec.z && vec.w == w);
}
bool vec4f::operator!=(const vec4f &vec) const
{
	return (x != vec.x || y != vec.y || z != vec.z || w != vec.w);
}
float vec4f::Dot3(const vec4f &vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}
float vec4f::Dot4(const vec4f &vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z) + (w * vec.w);
}

vec4f vec4f::Cross(const vec4f &vec) const
{
	return vec4f((y * vec.z) - (z * vec.y),
					(z * vec.x) - (x * vec.z),
					(x * vec.y) - (y * vec.x));
}
float vec4f::Distance(const vec4f &vec) const
{
	vec4f dvec(vec.x - x, vec.y - y, vec.z - z);
	return sqrtf(dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z);
}
float vec4f::DistanceSq(const vec4f &vec) const
{
	vec4f dvec(vec.x - x, vec.y - y, vec.z - z);
	return dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z;
}
float vec4f::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}
float vec4f::LengthSq() const
{
	return x * x + y * y + z * z;
}
vec4f &vec4f::Normalize()
{
	float length = sqrtf(x * x + y * y + z * z);
	if(length > 0)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	return *this;
}
void vec4f::Reverse()
{
	x = -x;
	y = -y;
	z = -z;
}

///////////////////////////////////////////////////////////////////////////////
//    Quaternion
///////////////////////////////////////////////////////////////////////////////
const Quaternion Quaternion::identity(0, 0, 0, 1);

Quaternion::Quaternion()
{
	v.set(0.0f, 0.0f, 0.0f);
	s = 0.0f;
}

Quaternion::Quaternion(float real, const vec3f &i)
	: s(real), v(i)
{

}

Quaternion::Quaternion(float real, float x, float y, float z)
	: s(real), v(x, y, z)
{

}

//! from 3 euler angles 
Quaternion::Quaternion(const vec3f &angles)
{
	float cos_z_2 = cosf(0.5f * angles.z);
	float cos_y_2 = cosf(0.5f * angles.y);
	float cos_x_2 = cosf(0.5f * angles.x);
		
	float sin_z_2 = sinf(0.5f * angles.z);
	float sin_y_2 = sinf(0.5f * angles.y);
	float sin_x_2 = sinf(0.5f * angles.x);

	// and now compute quaternion
	s   = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
	v.x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
	v.y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
	v.z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
}

Quaternion::Quaternion(float theta_z, float theta_y, float theta_x)
{
	float cos_z_2 = cosf(0.5f * theta_z);
	float cos_y_2 = cosf(0.5f * theta_y);
	float cos_x_2 = cosf(0.5f * theta_x);
		
	float sin_z_2 = sinf(0.5f * theta_z);
	float sin_y_2 = sinf(0.5f * theta_y);
	float sin_x_2 = sinf(0.5f * theta_x);
		
	// and now compute quaternion
	s   = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
	v.x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
	v.y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
	v.z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
}

void Quaternion::Set(float x, float y, float z, float s)
{
	v.x = x;
	v.y = y;
	v.z = z;
	this->s = s;
}

void Quaternion::Set(float theta_z, float theta_y, float theta_x)
{
	float cos_z_2 = cosf(0.5f * theta_z);
	float cos_y_2 = cosf(0.5f * theta_y);
	float cos_x_2 = cosf(0.5f * theta_x);
		
	float sin_z_2 = sinf(0.5f * theta_z);
	float sin_y_2 = sinf(0.5f * theta_y);
	float sin_x_2 = sinf(0.5f * theta_x);
		
	// and now compute quaternion
	s   = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
	v.x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
	v.y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
	v.z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
}

void Quaternion::SetIdentity()
{
	v.set(0.0f, 0.0f, 0.0f);
	s = 1.0f;
}

//! basic operations
Quaternion &Quaternion::operator=(const Quaternion &q)
{
	s = q.s;
	v = q.v;
	return *this;
}
	
const Quaternion Quaternion::operator+(const Quaternion &q) const
{
	return Quaternion(s + q.s, v + q.v);
}
	
const Quaternion Quaternion::operator-(const Quaternion &q) const
{
	return Quaternion(s - q.s, v - q.v);
}
	
const Quaternion Quaternion::operator*(const Quaternion &q) const
{
	return Quaternion(s * q.s - v * q.v,
                        v.y * q.v.z - v.z * q.v.y + s * q.v.x + v.x * q.s,
                        v.z * q.v.x - v.x * q.v.z + s * q.v.y + v.y * q.s,
                        v.x * q.v.y - v.y * q.v.x + s * q.v.z + v.z * q.s);
}

const Quaternion Quaternion::operator/(const Quaternion &q) const
{
	Quaternion p(q);
	p.Invert();
	return *this * p;
}

const Quaternion Quaternion::operator*(float scale) const
{
	return Quaternion(s * scale, v * scale);
}

const Quaternion Quaternion::operator/(float scale) const
{
	return Quaternion(s / scale, v / scale);
}

const Quaternion Quaternion::operator-() const
{
	return Quaternion(-s, -v);
}

const Quaternion &Quaternion::operator+=(const Quaternion &q)
{
	v += q.v;
	s += q.s;
	return *this;
}

const Quaternion &Quaternion::operator-=(const Quaternion &q)
{
	v -= q.v;
	s -= q.s;
	return *this;
}

const Quaternion &Quaternion::operator*=(const Quaternion &q)
{			
	s= s * q.s - v * q.v;
		
	float x = v.x;
	float y = v.y;
	float z = v.z;

	v.x = y * q.v.z - z * q.v.y + s * q.v.x + x * q.s;
	v.y = z * q.v.x - x * q.v.z + s * q.v.y + y * q.s;
	v.z = x * q.v.y - y * q.v.x + s * q.v.z + z * q.s;
	return *this;
}

const Quaternion &Quaternion::operator*=(float scale)
{
	v *= scale;
	s *= scale;
	return *this;
}

const Quaternion &Quaternion::operator/=(float scale)
{
	v /= scale;
	s /= scale;
	return *this;
}

Quaternion::operator mat3f() const
{
	//quaternion is not normalized
	assert(Length() > 0.9999 && Length() < 1.0001);

	return mat3f(1 - 2 * (v.y * v.y + v.z * v.z),
					2 * (v.x * v.y - s * v.z),
					2 * (v.x * v.z + s * v.y),
					2 * (v.x * v.y + s * v.z),
					1 - 2 * (v.x * v.x + v.z * v.z),
					2 * (v.y * v.z - s * v.x),
					2 * (v.x * v.z - s * v.y),
					2 * (v.y * v.z + s * v.x),
					1 - 2 * (v.x * v.x + v.y * v.y));
}

Quaternion::operator mat4f() const
{
	float xx = v.x * v.x;  float xy = v.x * v.y;  float xz = v.x * v.z;  float xw = v.x * s;
    float yy = v.y * v.y;  float yz = v.y * v.z;  float yw = v.y * s;
    float zz = v.z * v.z;  float zw = v.z * s;
		
	return mat4f(1 - 2 * (zz + yy),		2 * (xy - zw),		2 * (yw + xz),		0,
					2 * (xy + zw), 		1 - 2 * (zz + xx),	2 * (yz - xw),		0,
					2 * (xz - yw), 		2 * (yz + xw),		1 - 2 * (yy + xx),	0,
					0, 					0,					0,					1);
}

mat4f Quaternion::ToMatrix() const
{
	float xx = v.x * v.x;  float xy = v.x * v.y;  float xz = v.x * v.z;  float xw = v.x * s;
    float yy = v.y * v.y;  float yz = v.y * v.z;  float yw = v.y * s;
    float zz = v.z * v.z;  float zw = v.z * s;
		
	return mat4f(1 - 2 * (zz + yy),		2 * (xy - zw),		2 * (yw + xz),		0,
					2 * (xy + zw), 		1 - 2 * (zz + xx),	2 * (yz - xw),		0,
					2 * (xz - yw), 		2 * (yz + xw),		1 - 2 * (yy + xx),	0,
					0, 					0,					0,					1);
}

//! gets the length of this quaternion
float Quaternion::Length() const
{
	return (float)sqrtf(s * s + v * v);
}

//! gets the squared length of this quaternion
float Quaternion::LengthSq() const
{
	return (float)(s * s + v * v);
}

//! normalizes this quaternion
void Quaternion::Normalize()
{
	(*this) /= Length();
}

//! returns the normalized version of this quaternion
Quaternion Quaternion::Normalized() const
{
	return (*this) / Length();
}

//! computes the conjugate of this quaternion
void Quaternion::Conjugate()
{
	v = -v;
}

//! inverts this quaternion
void Quaternion::Invert()
{
	Conjugate();
	*this /= LengthSq();
}

//! returns the logarithm of a quaternion = (v * a) where q = [cos(a), v * sin(a)]
Quaternion Quaternion::Log() const
{
	float a = acosf(s);
	float sina = sinf(a);
	Quaternion ret;

	ret.s = 0;
	if (sina > 0)
	{
		ret.v.x = a * v.x / sina;
		ret.v.y = a * v.y / sina;
		ret.v.z = a * v.z / sina;
	}
	else
	{
		ret.v.x = 0;
		ret.v.y = 0;
		ret.v.z = 0;
	}

	return ret;
}

//! returns e^quaternion = exp(v*a) = [cos(a),vsin(a)]
Quaternion Quaternion::Exp() const
{
	float a = v.Length();
	float sina = sinf(a);
	float cosa = cosf(a);
	Quaternion ret;

	ret.s = cosa;

	if (a > 0)
	{
		ret.v.x = sina * v.x / a;
		ret.v.y = sina * v.y / a;
		ret.v.z = sina * v.z / a;
	}
	else
	{
		ret.v.x = 0;
		ret.v.y = 0;
		ret.v.z = 0;
	}

	return ret;
}

inline float Quaternion::Dot(const Quaternion &q1, const Quaternion &q2) 
{
	return q1.v * q2.v + q1.s * q2.s;
}

Quaternion Quaternion::Lerp(const Quaternion &q1, const Quaternion &q2, float t)
{
	return (q1 * (1 - t) + q2 * t).Normalized();
}

Quaternion Quaternion::Slerp(const Quaternion &q1, const Quaternion &q2, float t)
{
	Quaternion q3;
	float dot = Quaternion::Dot(q1, q2);
	
	if (dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	}
	else
	{
		q3 = q2;
	}
		
	if (dot < 0.95f)
	{
		float angle = acosf(dot);
		return (q1 * sinf(angle * (1 - t)) + q3 * sinf(angle * t)) / sinf(angle);
	}
	else
	{
		return Lerp(q1, q3, t);
	}
}

Quaternion Quaternion::SlerpNoInvert(const Quaternion &q1, const Quaternion &q2, float t)
{
	float dot = Quaternion::Dot(q1, q2);

	if (dot > -0.95f && dot < 0.95f)
	{
		float angle = acosf(dot);
		return (q1 * sinf(angle * (1 - t)) + q2 * sinf(angle * t)) / sinf(angle);
	}
	else
	{
		return Lerp(q1, q2, t);
	}
}

Quaternion Quaternion::Squad(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t)
{
	Quaternion c = SlerpNoInvert(q1, q2, t),
				d = SlerpNoInvert(a, b, t);
		
	return SlerpNoInvert(c, d, 2 * t * (1 - t));
}

Quaternion Quaternion::Bezier(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t)
{
	Quaternion q11 = SlerpNoInvert(q1, a, t),
				q12 = SlerpNoInvert(a, b, t),
				q13 = SlerpNoInvert(b, q2, t);
	
	return SlerpNoInvert(SlerpNoInvert(q11, q12, t), SlerpNoInvert(q12, q13, t), t);
}

Quaternion Quaternion::Spline(const Quaternion &qnm1,const Quaternion &qn,const Quaternion &qnp1)
{
	Quaternion qni(qn.s, -qn.v);
	return qn * (( (qni * qnm1).Log() + (qni * qnp1).Log() ) / -4).Exp();
}

Quaternion Quaternion::FromAxisAngle(const vec3f &axis, float angle)
{
	return Quaternion(cosf(angle / 2), axis * sinf(angle / 2));
}

void Quaternion::ToAxisAngle(vec3f &axis, float &angle) const
{
	angle = acosf(s);

	float sinf_theta_inv = 1.0f / sinf(angle);

	// now the vector
	axis.x = v.x * sinf_theta_inv;
	axis.y = v.y * sinf_theta_inv;
	axis.z = v.z * sinf_theta_inv;

	angle *= 2;
}

vec3f Quaternion::Rotate(const vec3f &v)
{
	Quaternion V(0, v);
	Quaternion conjugate(*this);
	conjugate.Conjugate();
	return (*this * V * conjugate).v;
}

vec3f Quaternion::Rotate(float x, float y, float z)
{
	Quaternion V(0, x, y, z);
	Quaternion conjugate(*this);
	conjugate.Conjugate();
	return (*this * V * conjugate).v;
}

vec3f Quaternion::EulerAngles(bool homogenous) const
{
	const float sqw = s * s;
	const float sqx = v.x * v.x;
	const float sqy = v.y * v.y;
	const float sqz = v.z * v.z;

	vec3f euler;

	if(homogenous)
	{
		euler.x = atan2f(2.f * (v.x * v.y + v.z * s), sqx - sqy - sqz + sqw);
		euler.y = asinf(-2.f * (v.x * v.z - v.y * s));
		euler.z = atan2f(2.f * (v.y * v.z + v.x * s), -sqx - sqy + sqz + sqw);
	}
	else
	{
		euler.x = atan2f(2.f * (v.z * v.y + v.x * s), 1 - 2 * (sqx + sqy));
		euler.y = asinf(-2.f * (v.x * v.z - v.y * s));
		euler.z = atan2f(2.f * (v.x * v.y + v.z * s), 1 - 2 * (sqy + sqz));
	}

	return euler;
}

//////////////////////////////////////
//    Color
//////////////////////////////////////

Color Color::red(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::green(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::blue(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::yellow(1.0f, 1.0f, 0.0f, 1.0f);
Color Color::orange(1.0f, 0.5f, 0.0f, 1.0f);
Color Color::purple(0.5f, 0.0f, 0.5f, 1.0f);
Color Color::magenta(1.0f, 0.0f, 1.0f, 1.0f);
Color Color::cyan(0.0f, 1.0f, 1.0f, 1.0f);
Color Color::black(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::white(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::gray(0.5f, 0.5f, 0.5f, 1.0f);
Color Color::clear(0.0f, 0.0f, 0.0f, 0.0f);

Color &Color::operator=(const vec4f &color)
{
	r = color.x;
	g = color.y;
	b = color.z;
	a = color.w;
	return *this;
}

Color::operator uint32_t()
{
	uint32_t red = ((uint32_t)(r * 255.0f) & 0xFF) << 24;
	uint32_t grn = ((uint32_t)(g * 255.0f) & 0xFF) << 16;
	uint32_t blu = ((uint32_t)(b * 255.0f) & 0xFF) << 8;
	uint32_t alp = ((uint32_t)(a * 255.0f) & 0xFF);
	return red | grn | blu | alp;
}

Color::operator vec4f&()
{
	return *(vec4f*)this;
}

Color::operator float*()
{
	return (float*)this;
}

uint32_t Color::RGBA(uint8_t r, uint8_t g, uint8_t b,  uint8_t a)
{
	uint32_t red = (uint32_t)r;
	uint32_t grn = (uint32_t)g << 8;
	uint32_t blu = (uint32_t)b << 16;
	uint32_t alp = (uint32_t)a << 24;
	
	return red | grn | blu | alp;
}

/////////////////////

float _invRandMax = 1.0f / (float)RAND_MAX;

float Random::value()
{
	return (float)rand() * _invRandMax;
}

float Random::signedValue()
{
	return (float)rand() * _invRandMax * 2.0f - 1.0f;
}

vec2f Random::vector2()
{
	float a = value() * math::twopi;
	float r = value() + value();
	r = r > 1 ? 2 - r : r;

	return vec2f(r * cos(a), r * sin(a));
}

float Random::range(float min, float max)
{
	return min + (float)rand() * _invRandMax * (max - min);
}
