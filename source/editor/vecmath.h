/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_VECMATH_H
#define INC_VECMATH_H

/*************************************************************************

 This file uses OpenGL style column major, left operation, matrix notation

**************************************************************************/

#include <math.h>

#define PI					(3.141592654f)
#define HALFPI				(PI / 2.0f)
#define TWOPI				(PI * 2.0f)
#define RADIANS(x)			((float)x * (PI / 180.0f))
#define DEGREES(x)			((float)x * (180.0f / PI))

typedef char			Int8;
typedef short			Int16;
typedef int				Int32;
typedef unsigned char	Uint8;
typedef unsigned short	Uint16;
typedef unsigned int	Uint32;

template<class T>
inline T Min4(const T &a, const T &b, const T &c, const T &d)
{
	T m = a;
	if(b < m) m = b;
	if(c < m) m = c;
	if(d < m) return d;
	return m;
}

template<class T>
inline T Max4(const T &a, const T &b, const T &c, const T &d)
{
	T m = a;
	if(b > m) m = b;
	if(c > m) m = c;
	if(d > m) return d;
	return m;
}

template<class T>
class Rect
{
public:
	T x, y, w, h;

	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(T X, T Y, T W, T H) : x(X), y(Y), w(W), h(H) {}

	inline void Set(T X, T Y, T W, T H)
	{
		x = X;
		y = Y;
		w = W;
		h = H;
	}

	inline void Offset(T offset_x = 0, T offset_y = 0)
	{
		x += offset_x;
		y += offset_y;
	}
	
	inline void Deflate(T px = 0, T py = 0)
	{
		x += px;
		y += py;
		w -= 2*px;
		h -= 2*py;
	}

	inline bool Intersects(Rect &rect) const
	{
		return !(x > rect.x + rect.w
			  || x + w < rect.x
			  || y > rect.y + rect.h
			  || y + h < rect.y);
	}

	inline bool PointInside(T X, T Y) const
	{
		return !(X < x
			  || X > x + w - 1
			  || Y < y
			  || Y > y + h - 1);
	}
};

template<class T>
class Quad // drawable as a strip
{
public:
	T TL; // top left
	T BL; // bottom left
	T BR; // bottom right
	T TR; // top right
};

// forward declaration for use by matrix operators
template<class T> class Vec2;
template<class T> class Vec3;
template<class T> class Vec4;

// mat 3 is row major
template<class T>
class Mat3
{
public:
	
	T m11, m12, m13, //column 1
	  m21, m22, m23, //column 2
	  m31, m32, m33; //column 3

	Mat3(){}

	Mat3(bool loadIdentity)
	{
		if(loadIdentity) SetIdentity();
	}

	Mat3(T _11, T _21, T _31,
		 T _12, T _22, T _32,
		 T _13, T _23, T _33)

	   :m11(_11), m21(_21), m31(_31),
		m12(_12), m22(_22), m32(_32),
		m13(_13), m23(_23), m33(_33){}
	
	inline void Set(T _11, T _21, T _31,
			 T _12, T _22, T _32,
			 T _13, T _23, T _33)
	{
		m11 = _11; m21 = _21; m31 = _31;
		m12 = _12; m22 = _22; m32 = _32;
		m13 = _13; m23 = _23; m33 = _33;
	}

	inline Vec2<T> operator*(const Vec2<T> &V) const
	{
		return Vec2<T>(m11 * V.x + m21 * V.y + m31,
					   m12 * V.x + m22 * V.y + m32);
	}

	inline Vec3<T> operator*(const Vec3<T> &V) const
	{
		return Vec3<T>(m11 * V.x + m21 * V.y + m31 * V.z,
					   m12 * V.x + m22 * V.y + m32 * V.z,
					   m13 * V.x + m23 * V.y + m33 * V.z);
	}

	inline Mat3 operator*(float s)
	{
		return Mat3(m11 * s, m21 * s, m31 * s,
					m12 * s, m22 * s, m32 * s,
					m13 * s, m23 * s, m33 * s);
	}

	inline Mat3 operator*=(float s)
	{
		m11 *= s;
		m12 *= s;
		m13 *= s;

		m21 *= s;
		m22 *= s;
		m23 *= s;

		m31 *= s;
		m32 *= s;
		m33 *= s;
	}

	inline Mat3 operator*(const Mat3 &M) const
	{
		return Mat3(m11 * M.m11 + m21 * M.m12 + m31 * M.m13,
					m11 * M.m21 + m21 * M.m22 + m31 * M.m23,
					m11 * M.m31 + m21 * M.m32 + m31 * M.m33,

					m12 * M.m11 + m22 * M.m12 + m32 * M.m13,
					m12 * M.m21 + m22 * M.m22 + m32 * M.m23,
					m12 * M.m31 + m22 * M.m32 + m32 * M.m33,
					
					m13 * M.m11 + m23 * M.m12 + m33 * M.m13,
					m13 * M.m21 + m23 * M.m22 + m33 * M.m23,
					m13 * M.m31 + m23 * M.m32 + m33 * M.m33);
	}
	
	inline Mat3 &operator*=(const Mat3 &M)
	{
		Set(m11 * M.m11 + m21 * M.m12 + m31 * M.m13,
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

	inline void Translate(T x, T y)
	{
		*this = Mat3::Translation(x, y) * (*this);
	}

	inline bool Invert()
	{
		//calculate matrix of minors -> matrix of cofactors-> adjugate
		Mat3<T> m(m22 * m33 - m32 * m23, -(m21 * m33 - m31 * m23),  m21 * m32 - m31 * m22,
				-(m12 * m33 - m32 * m13),  m11 * m33 - m31 * m13, -(m11 * m32 - m31 * m12),
				  m12 * m23 - m22 * m13, -(m11 * m23 - m21 * m13),  m11 * m22 - m21 * m12);

		// calculate determinant
		T det = m11 * m.m11
			  + m21 * m.m12
			  + m31 * m.m13;

		if(det == 0)
			return false;

		// divide by determinant and return
		*this = m * (1.0f / det);

		return true;
	}

	inline static bool Invert(const Mat3 &m, Mat3 &out)
	{
		Mat3<T> i(m.m22 * m.m33 - m.m32 * m.m23, -(m.m21 * m.m33 - m.m31 * m.m23),  m.m21 * m.m32 - m.m31 * m.m22,
				-(m.m12 * m.m33 - m.m32 * m.m13),  m.m11 * m.m33 - m.m31 * m.m13, -(m.m11 * m.m32 - m.m31 * m.m12),
				  m.m12 * m.m23 - m.m22 * m.m13, -(m.m11 * m.m23 - m.m21 * m.m13),  m.m11 * m.m22 - m.m21 * m.m12);

		T det = m.m11 * i.m11
			  + m.m21 * i.m12
			  + m.m31 * i.m13;

		if(det == 0)
			return false;

		out = m * (1.0f / det);

		return true;
	}

	inline void SetOrtho2D(T left, T right, T bottom, T top)
	{
		T dx = right - left;
		T dy = top - bottom;

		T tx = (dx != 0) ? -(right + left) / dx : 0;
		T ty = (dy != 0) ? -(top + bottom) / dy : 0;

		Set(2.0f / dx,  0,			tx,
			0,			2.0f / dy,  ty,
			0,			0,			1);
	}

	inline void SetIdentity()
	{
		Set(1, 0, 0,
			0, 1, 0,
			0, 0, 1);
	}

	inline void SetTranslation(T x, T y)
	{
		Set(1, 0, x,
			0, 1, y,
			0, 0, 1);
	}

	inline void SetScale(T sx, T sy)
	{
		Set(sx, 0, 0,
			0, sy, 0,
			0, 0, 1);
	}

	inline void SetRotation(T angle)
	{
		T radAng = RADIANS(angle);
		T cf = cosf(radAng);
		T sf = sinf(radAng);

		Set(cf,	-sf, 0,
			sf, cf,  0,
			0,	0,   1);
	}

	inline void Transpose()
	{
		Set(m11, m12, m13,
			m21, m22, m23,
			m31, m32, m33);
	}

	inline static Mat3 Scale(T sx = 1, T sy = 1, T sw = 1)
	{
		return Mat3(sx, 0,  0,
					0,  sy, 0,
					0,  0,  sw);
	}

	inline static Mat3 Translation(T x, T y)
	{
		return Mat3(1, 0, x,
					0, 1, y,
					0, 0, 1);
	}

	inline static Mat3 Identity()
	{
		return Mat3(1, 0, 0,
					0, 1, 0,
					0, 0, 1);
	}

	inline static Mat3 Ortho2D(T left, T right, T bottom, T top)
	{
		T dx = right - left;
		T dy = top - bottom;

		T tx = (dx != 0) ? -(right + left) / dx : 0;
		T ty = (dy != 0) ? -(top + bottom) / dy : 0;
		
		return Mat3(2.0f / dx, 0,			tx,
					0,			2.0f / dy,  ty,
					0,			0,			1);
	}
};

template<class T>
class Mat4
{
public:

	T m11, m12, m13, m14, //column 1
	  m21, m22, m23, m24, //column 2
	  m31, m32, m33, m34, //column 3
	  m41, m42, m43, m44; //column 4
	
	Mat4(){}
	Mat4(bool loadIdentity)
	{
		if(loadIdentity) SetIdentity();
	}

	Mat4(T _11, T _21, T _31, T _41,
		 T _12, T _22, T _32, T _42,
		 T _13, T _23, T _33, T _43,
		 T _14, T _24, T _34, T _44)

		 :m11(_11), m21(_21), m31(_31), m41(_41),
		  m12(_12), m22(_22), m32(_32), m42(_42),
		  m13(_13), m23(_23), m33(_33), m43(_43),
		  m14(_14), m24(_24), m34(_34), m44(_44){}
	
	inline void Set(T _11, T _21, T _31, T _41,
			 T _12, T _22, T _32, T _42,
			 T _13, T _23, T _33, T _43,
			 T _14, T _24, T _34, T _44)
	{
		m11 = _11; m21 = _21; m31 = _31; m41 = _41;
		m12 = _12; m22 = _22; m32 = _32; m42 = _42;
		m13 = _13; m23 = _23; m33 = _33; m43 = _43;
		m14 = _14; m24 = _24; m34 = _34; m44 = _44;
	}

	inline Vec4<T> operator*(const Vec4<T> &V) const
	{
		return Vec3<T>(m11 * V.x + m21 * V.y + m31 * V.z + m41 * V.w,
					   m12 * V.x + m22 * V.y + m32 * V.z + m42 * V.w,
					   m13 * V.x + m23 * V.y + m33 * V.z + m43 * V.w,
					   m14 * V.x + m24 * V.y + m34 * V.z + m44 * V.w);
	}

	inline Mat4 operator*(float s)
	{
		return Mat4(m11 * s, m21 * s, m31 * s, m41 * s,
					m12 * s, m22 * s, m32 * s, m42 * s,
					m13 * s, m23 * s, m33 * s, m43 * s,
					m14 * s, m24 * s, m34 * s, m44 * s);
	}

	inline Mat4 operator*=(float s)
	{
		m11 *= s;
		m12 *= s;
		m13 *= s;
		m14 *= s;

		m21 *= s;
		m22 *= s;
		m23 *= s;
		m24 *= s;

		m31 *= s;
		m32 *= s;
		m33 *= s;
		m34 *= s;

		m41 *= s;
		m42 *= s;
		m43 *= s;
		m44 *= s;
	}

	inline Mat4 operator*(const Mat4 &M) const
	{
		return Mat4(m11 * M.m11 + m21 * M.m12 + m31 * M.m13 + m41 * M.m14,
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

	inline Mat4 &operator*=(const Mat4 &M)
	{
		Set(m11 * M.m11 + m21 * M.m12 + m31 * M.m13 + m41 * M.m14,
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

	inline void Translate(T x, T y, T z)
	{
		*this = Mat4::Translation(x, y, z) * (*this);
	}
	
	inline bool Invert()
	{
		Mat4<T> tmp;

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

		T det = m11 * tmp.m11 + m12 * tmp.m21 + m13 * tmp.m31 + m14 * tmp.m41;

		if(det == 0)
			return false;

		*this = tmp * (1.0f / det);

		return true;
	}

	inline static bool Invert(const Mat4<float> &m, Mat4<float> &out)
	{
		Mat4<T> tmp;

		tmp.m11 = m.m22 * m.m33 * m.m44 - m.m22 * m.m34 * m.m43 - m.m32 * m.m23 * m.m44 + m.m32 * m.m24 * m.m43 + m.m42 * m.m23 * m.m34 - m.m42 * m.m24 * m.m33;
		tmp.m21 = -m.m21 * m.m33 * m.m44 + m.m21 * m.m34 * m.m43 + m.m31 * m.m23 * m.m44 - m.m31 * m.m24 * m.m43 - m.m41 * m.m23 * m.m34 + m.m41 * m.m24 * m.m33;
		tmp.m31 = m.m21  *  m.m32 * m.m44 - m.m21 * m.m34 * m.m42 - m.m31 * m.m22 * m.m44 + m.m31 * m.m24 * m.m42 + m.m41 * m.m22 * m.m34 - m.m41 * m.m24 * m.m32;
		tmp.m41 = -m.m21 * m.m32 * m.m43 + m.m21 * m.m33 * m.m42 + m.m31 * m.m22 * m.m43 - m.m31 * m.m23 * m.m42 - m.m41 * m.m22 * m.m33 + m.m41 * m.m23 * m.m32;
		tmp.m12 = -m.m12 * m.m33 * m.m44 + m.m12 * m.m34 * m.m43 + m.m32 * m.m13 * m.m44 - m.m32 * m.m14 * m.m43 - m.m42 * m.m13 * m.m34 + m.m42 * m.m14 * m.m33;
		tmp.m22 = m.m11 * m.m33 * m.m44 - m.m11 * m.m34 * m.m43 - m.m31 * m.m13 * m.m44 + m.m31 * m.m14 * m.m43 + m.m41 * m.m13 * m.m34 - m.m41 * m.m14 * m.m33;
		tmp.m32 = -m.m11 * m.m32 * m.m44 + m.m11 * m.m34 * m.m42 + m.m31 * m.m12 * m.m44 - m.m31 * m.m14 * m.m42 - m.m41 * m.m12 * m.m34 + m.m41 * m.m14 * m.m32;
		tmp.m42 = m.m11 * m.m32 * m.m43 - m.m11 * m.m33 * m.m42 - m.m31 * m.m12 * m.m43 + m.m31 * m.m13 * m.m42 + m.m41 * m.m12 * m.m33 - m.m41 * m.m13 * m.m32;
		tmp.m13 = m.m12 * m.m23 * m.m44 - m.m12 * m.m24 * m.m43 - m.m22 * m.m13 * m.m44 + m.m22 * m.m14 * m.m43 + m.m42 * m.m13 * m.m24 - m.m42 * m.m14 * m.m23;
		tmp.m23 = -m.m11 * m.m23 * m.m44 + m.m11 * m.m24 * m.m43 + m.m21 * m.m13 * m.m44 - m.m21 * m.m14 * m.m43 - m.m41 * m.m13 * m.m24 + m.m41 * m.m14 * m.m23;
		tmp.m33 = m.m11 * m.m22 * m.m44 - m.m11 * m.m24 * m.m42 - m.m21 * m.m12 * m.m44 + m.m21 * m.m14 * m.m42 + m.m41 * m.m12 * m.m24 - m.m41 * m.m14 * m.m22;
		tmp.m43 = -m.m11 * m.m22 * m.m43 + m.m11 * m.m23 * m.m42 + m.m21 * m.m12 * m.m43 - m.m21 * m.m13 * m.m42 - m.m41 * m.m12 * m.m23 + m.m41 * m.m13 * m.m22;
		tmp.m14 = -m.m12 * m.m23 * m.m34 + m.m12 * m.m24 * m.m33 + m.m22 * m.m13 * m.m34 - m.m22 * m.m14 * m.m33 - m.m32 * m.m13 * m.m24 + m.m32 * m.m14 * m.m23;
		tmp.m24 = m.m11 * m.m23 * m.m34 - m.m11 * m.m24 * m.m33 - m.m21 * m.m13 * m.m34 + m.m21 * m.m14 * m.m33 + m.m31 * m.m13 * m.m24 - m.m31 * m.m14 * m.m23;
		tmp.m34 = -m.m11 * m.m22 * m.m34 + m.m11 * m.m24 * m.m32 + m.m21 * m.m12 * m.m34 - m.m21 * m.m14 * m.m32 - m.m31 * m.m12 * m.m24 + m.m31 * m.m14 * m.m22;
		tmp.m44 = m.m11 * m.m22 * m.m33 - m.m11 * m.m23 * m.m32 - m.m21 * m.m12 * m.m33 + m.m21 * m.m13 * m.m32 + m.m31 * m.m12 * m.m23 - m.m31 * m.m13 * m.m22;

		T det = m.m11 * tmp.m11 + m.m12 * tmp.m21 + m.m13 * tmp.m31 + m.m14 * tmp.m41;

		if(det == 0)
			return false;

		out = tmp * (1.0 / det);

		return true;
	}

	inline void SetOrtho2D(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		T dx = right - left;
		T dy = top - bottom;
		T dz = zFar - zNear;

		T tx = (dx != 0) ? -(right + left) / dx : 0;
		T ty = (dy != 0) ? -(top + bottom) / dy : 0;
		T tz = (dz != 0) ? -(zFar + zNear) / dz : 0;

		Set(2.0f / dx, 0,			0,			tx,
			0,			2.0f / dy, 0,			ty,
			0,			0,			-2.0f / dz, tz,
			0,			0,			0,			1);
	}

	inline void SetProject3D(T fov, T view_w, T view_h, T ZNear, T ZFar)
	{
		T Delta = ZFar - ZNear;

		T sx = 1.0f / tanf(RADIANS(fov / 2.0f));
		T sy = sx / (view_h / view_w);
		T sz = -(ZFar + ZNear) / Delta;

		Set(sx,			0,			0,			0,
			0,			sy,			0,			0,
			0,			0,			sz,			-2.0f * ZFar * ZNear / Delta,
			0,			0,			-1.0,		0);
	}

	inline void SetIdentity()
	{
		Set(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	inline void SetTranslation(T x, T y, T z)
	{
		Set(1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1);
	}

	inline void SetScale(T sx = 1, T sy = 1, T sz = 1, T sw = 1)
	{
		Set(sx, 0, 0, 0,
			0, sy, 0, 0,
			0, 0, sz, 0,
			0, 0, 0, sw);
	}

	inline void SetRotationX(T angle)
	{
		float radAng = RADIANS(angle);
		float cf = cosf(radAng);
		float sf = sinf(radAng);
		Set(1, 0, 0, 0,
			0, cf, -sf, 0,
			0, sf, cf, 0,
			0, 0, 0, 1);
	}

	inline void SetRotationY(T angle)
	{
		float radAng = RADIANS(angle);
		float cf = cosf(radAng);
		float sf = sinf(radAng);
		Set(cf, 0, sf, 0,
			0, 1, 0, 0,
			-sf, 0, cf, 0,
			0, 0, 0, 1);
	}

	inline void SetRotationZ(T angle)
	{
		float radAng = RADIANS(angle);
		float cf = cosf(radAng);
		float sf = sinf(radAng);
		Set(cf, -sf, 0, 0,
			sf, cf, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	// add rotation functions

	inline void Transpose()
	{
		Set(m11, m12, m13, m14,
			m21, m22, m23, m24,
			m31, m32, m33, m34,
			m41, m42, m43, m44);
	}

	inline static Mat4 Scale(T sx = 1, T sy = 1, T sz = 1, T sw = 1)
	{
		return Mat4(sx, 0, 0, 0,
					 0, sy, 0, 0,
					 0, 0, sz, 0,
					 0, 0, 0, sw);
	}

	inline static Mat4 Translation(T x, T y, T z)
	{
		return Mat4(1, 0, 0, x,
					 0, 1, 0, y,
					 0, 0, 1, z,
					 0, 0, 0, 1);
	}

	inline static Mat4 Identity()
	{
		return Mat4(1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1);
	}

	inline static Mat4 Ortho2D(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		T dx = right - left;
		T dy = top - bottom;
		T dz = zFar - zNear;

		T tx = (dx != 0) ? -(right + left) / dx : 0;
		T ty = (dy != 0) ? -(top + bottom) / dy : 0;
		T tz = (dz != 0) ? -(zFar + zNear) / dz : 0;

		return Mat4(2.0f / dx, 0,			0,			tx,
					0,			2.0f / dy, 0,			ty,
					0,			0,			-2.0f / dz, tz,
					0,			0,			0,			 1);
	}

	inline static Mat4 Project3D(T fov, T view_w, T view_h, T ZNear, T ZFar)
	{
		float Delta = ZFar - ZNear;

		T sx = 1.0f / tanf(RADIANS(fov / 2.0f));
		T sy = sx / (view_h / view_w);
		T sz = -(ZFar + ZNear) / Delta;

		return Mat4(sx,			0,			0,			0,
					0,			sy,			0,			0,
					0,			0,			sz,			-2.0f * ZFar * ZNear / Delta,
					0,			0,			-1.0,		0);
	}
};

template<class T>
class Vec2
{
public:
	T x, y;
	Vec2() : x(0), y(0){}
	Vec2(T _x, T _y) : x(_x), y(_y){}

	void setX(T x_) { this->x = x_; }
	void setY(T y_) { this->y = y_; }
	void set(T x_, T y_) {x = x_; y = y_;}

	inline Vec2<T> operator+(const Vec2 &vec) const
	{
		return Vec2(x + vec.x, y + vec.y);
	}
	inline Vec2 operator-(const Vec2 &vec) const
	{
		return Vec2(x - vec.x, y - vec.y);
	}
	inline Vec2 operator*(const T scalar) const
	{
		return Vec2(x * scalar, y * scalar);
	}
	inline T operator*(const Vec2 &vec) const
	{
		return x * vec.x + y * vec.y;
	}
	inline Vec2 operator/(const T scalar) const
	{
		return Vec2(x / scalar, y / scalar);
	}
	inline Vec2& operator+=(const Vec2 &vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}
	inline Vec2& operator-=(const Vec2 &vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}
	inline Vec2& operator*=(const T scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	inline Vec2 operator*(const Mat3<float> &M) const
	{
		return Vec2(x * M.m11 + y * M.m12 + M.m13,
					x * M.m21 + y * M.m22 + M.m23);
	}
	inline Vec2 &operator*=(const Mat3<float> &M)
	{
		return *this = Vec2(x * M.m11 + y * M.m12 + M.m13,
							x * M.m21 + y * M.m22 + M.m23);
	}
	inline Vec2 operator*(const Mat4<float> &M) const
	{
		return Vec2(x * M.m11 + y * M.m12 + M.m14,
					x * M.m21 + y * M.m22 + M.m24);
	}
	inline Vec2 &operator*=(const Mat4<float> &M)
	{
		return *this = Vec2(x * M.m11 + y * M.m12 + M.m14,
							x * M.m21 + y * M.m22 + M.m24);
	}
	inline void LeftTransform(const Mat3<float> &M, const Vec2 &V)
	{
		x = M.m11 * V.x + M.m21 * V.y + M.m31;
		y = M.m12 * V.x + M.m22 * V.y + M.m32;
	}
	inline void LeftTransform(const Mat4<float> &M, const Vec2 &V)
	{
		x = M.m11 * V.x + M.m21 * V.y + M.m41;
		y = M.m12 * V.x + M.m22 * V.y + M.m42;
	}
	inline Vec2& operator/=(const T scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
	inline bool operator==(const Vec2 &vec) const
	{
		return (x == vec.x && y == vec.y);
	}
	inline bool operator!=(const Vec2 &vec) const
	{
		return (x != vec.x || y != vec.y);
	}
	inline Vec2 Perp()  const// clockwise 90 degrees
	{
		return Vec2(y, -x);
	}
	inline T Distance(const Vec2 &vec) const
	{
		Vec2 dvec(vec.x - x, vec.y - y);
		return sqrt((dvec.x * dvec.x) + (dvec.y * dvec.y));
	}
	inline T DistanceSq(const Vec2 &vec) const
	{
		Vec2 dvec(vec.x - x, vec.y - y);
		return (dvec.x * dvec.x) + (dvec.y * dvec.y);
	}
	inline T Length() const
	{
		return sqrtf((x * x) + (y * y));
	}
	inline T LengthSq() const
	{
		return (x * x) + (y * y);
	}
	inline T Det(const Vec2 &vec) const
	{
		return x * vec.y - vec.x * y;
	}

	// normalize vector , doesn't return or save the length
	inline void Normalize()
	{
		T length = sqrtf(x * x + y * y);
		if(length > 0)
		{
			x /= length;
			y /= length;
		}
	}

	inline void Reverse()
	{
		x = -x;
		y = -y;
	}
};

template<class T>
class Vec3
{
public:
	T x, y, z;

	Vec3() : x(0), y(0), z(0){}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z){}
	
	void set(T x_, T y_, T z_) {x = x_; y = y_; z = z_;}

	Vec3 operator+(const Vec3 &vec) const
	{
		return Vec3(x + vec.x, y + vec.y, z + vec.z);
	}
	Vec3 operator-(const Vec3 &vec) const
	{
		return Vec3(x - vec.x, y - vec.y, z - vec.z);
	}
	Vec3 operator*(const T scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}
	T operator*(const Vec3 &vec) const
	{
		return (x * vec.x) + (y * vec.y) + (z * vec.z);
	}
	Vec3 operator*(const Mat4<float> &M) const
	{
		return Vec3( x * M.m11 + y * M.m12 + z * M.m13 + M.m14,
					  x * M.m21 + y * M.m22 + z * M.m23 + M.m24,
					  x * M.m31 + y * M.m32 + z * M.m33 + M.m34);
	}
	Vec3 &operator*=(const Mat4<float> &M)
	{
		return *this = Vec3( x * M.m11 + y * M.m12 + z * M.m13 + M.m14,
							  x * M.m21 + y * M.m22 + z * M.m23 + M.m24,
							  x * M.m31 + y * M.m32 + z * M.m33 + M.m34);
	}
	Vec3 operator/(const T scalar) const
	{
		return Vec3(x / scalar, y / scalar, z / scalar);
	}
	Vec3& operator+=(const Vec3 &vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	Vec3& operator-=(const Vec3 &vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	Vec3& operator*=(const T scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	Vec3& operator/=(const T scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	bool operator==(const Vec3 &vec) const
	{
		return (x == vec.x && y == vec.y && z == vec.z);
	}
	bool operator!=(const Vec3 &vec) const
	{
		return (x != vec.x || y != vec.y || z != vec.z);
	}
	T Dot4(const Vec3 &vec) const
	{
		return (x * vec.x) + (y * vec.y) + (z * vec.z);
	}
	Vec3 Cross(const Vec3 &vec) const
	{
		return Vec3((y * vec.z) - (z * vec.y),
					 (z * vec.x) - (x * vec.z),
					 (x * vec.y) - (y * vec.x));
	}
	T Distance(const Vec3 &vec) const
	{
		Vec3 dvec(vec.x - x, vec.y - y, vec.z - z);
		return sqrtf(dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z);
	}
	T DistanceSq(const Vec3 &vec) const
	{
		Vec3 dvec(vec.x - x, vec.y - y, vec.z - z);
		return dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z;
	}
	T Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	T LengthSq() const
	{
		return x * x + y * y + z * z;
	}
	Vec3& Normalize()
	{
		T length = sqrtf(x * x + y * y + z * z);
		if(length > 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return *this;
	}
	void Reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
};

template<class T>
class Vec4
{
public:
	T x, y, z, w;

	Vec4() : x(0), y(0), z(0), w(1){}
	Vec4(T _x, T _y, T _z, T _w = 1) : x(_x), y(_y), z(_z), w(_w){}

	void set(T x_, T y_, T z_, T w_ = 1) {x = x_; y = y_; z = z_; w = w_;}

	Vec4 operator+(const Vec4 &vec) const
	{
		return Vec4(x + vec.x, y + vec.y, z + vec.z);
	}
	Vec4 operator-(const Vec4 &vec) const
	{
		return Vec4(x - vec.x, y - vec.y, z - vec.z);
	}
	Vec4 operator*(const T scalar) const
	{
		return Vec4(x * scalar, y * scalar, z * scalar);
	}
	T operator*(const Vec4 &vec) const
	{
		return (x * vec.x) + (y * vec.y) + (z * vec.z);
	}
	Vec4 operator*(const Mat4<T> &M) const
	{
		return Vec4(x * M.m11 + y * M.m12 + z * M.m13 + w * M.m14,
					x * M.m21 + y * M.m22 + z * M.m23 + w * M.m24,
					x * M.m31 + y * M.m32 + z * M.m33 + w * M.m34,
					x * M.m41 + y * M.m42 + z * M.m43 + w * M.m44);
	}
	Vec4 &operator*=(const Mat4<T> &M)
	{
		return *this = Vec4(x * M.m11 + y * M.m12 + z * M.m13 + w * M.m14,
							x * M.m21 + y * M.m22 + z * M.m23 + w * M.m24,
							x * M.m31 + y * M.m32 + z * M.m33 + w * M.m34,
							x * M.m41 + y * M.m42 + z * M.m43 + w * M.m44);
	}
	inline void LeftTransform(const Mat4<float> &M, const Vec4 &V)
	{
		x = M.m11 * V.x + M.m21 * V.y + M.m31 * V.z + M.m31 * V.w + M.m41;
		y = M.m12 * V.x + M.m22 * V.y + M.m32 * V.z + M.m32 * V.w + M.m42;
		z = M.m13 * V.x + M.m23 * V.y + M.m33 * V.z + M.m33 * V.w + M.m43;
		w = M.m14 * V.x + M.m24 * V.y + M.m34 * V.z + M.m34 * V.w + M.m44;
	}
	Vec4 operator/(const T scalar) const
	{
		return Vec4(x / scalar, y / scalar, z / scalar);
	}
	Vec4& operator+=(const Vec4 &vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	Vec4& operator-=(const Vec4 &vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	Vec4& operator*=(const T scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	Vec4& operator/=(const T scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	bool operator==(const Vec4 &vec) const
	{
		return (x == vec.x && y == vec.y && z == vec.z && vec.w == w);
	}
	bool operator!=(const Vec4 &vec) const
	{
		return (x != vec.x || y != vec.y || z != vec.z || w != vec.w);
	}
	T Dot4(const Vec4 &vec)
	{
		return (x * vec.x) + (y * vec.y) + (z * vec.z) + (w * vec.w);
	}

	Vec4 Cross(const Vec4 &vec) const
	{
		return Vec4((y * vec.z) - (z * vec.y),
					 (z * vec.x) - (x * vec.z),
					 (x * vec.y) - (y * vec.x));
	}
	T Distance(const Vec4 &vec) const
	{
		Vec4 dvec(vec.x - x, vec.y - y, vec.z - z);
		return sqrtf(dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z);
	}
	T DistanceSq(const Vec4 &vec) const
	{
		Vec4 dvec(vec.x - x, vec.y - y, vec.z - z);
		return dvec.x * dvec.x + dvec.y * dvec.y + dvec.z * dvec.z;
	}
	T Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	T LengthSq() const
	{
		return x * x + y * y + z * z;
	}
	Vec4& Normalize()
	{
		T length = sqrtf(x * x + y * y + z * z);
		if(length > 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return *this;
	}
	void Reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
};

class Color
{
public:

	Uint8 Red;
	Uint8 Green;
	Uint8 Blue;
	Uint8 Alpha;

	Color() : Red(0),
			  Green(0),
			  Blue(0),
			  Alpha(0){}
	Color(Uint8 Red, Uint8 Green, Uint8 Blue, Uint8 Alpha = 0xFF)
		: Red(Red),
		  Green(Green),
		  Blue(Blue),
		  Alpha(Alpha){}
	Color(Uint32 color)
	{
		Red = (Uint8)((color & 0xFF000000) >> 24);
		Green = (Uint8)((color & 0xFF0000) >> 16);
		Blue = (Uint8)((color & 0xFF00) >> 8);
		Alpha = (Uint8)((color & 0xFF));
	}

	void SetF(float Red, float Green, float Blue, float Alpha = 1.0f)
	{
		this->Red = (Uint8)(255.0f * Red + 0.5f);
		this->Green = (Uint8)(255.0f * Green + 0.5f);
		this->Blue = (Uint8)(255.0f * Blue + 0.5f);
		this->Alpha = (Uint8)(255.0f * Alpha + 0.5f);
	}

	operator Uint32()
	{
		return (Red << 24 | Green << 16 | Blue << 8 | Alpha);
	}

	float Redf() {return (float)Red / 255.0f;}
	float Greenf() {return (float)Green / 255.0f;}
	float Bluef() {return (float)Blue / 255.0f;}
	float Alphaf() {return (float)Alpha / 255.0f;}
};

#endif /* INC_VECMATH_H */