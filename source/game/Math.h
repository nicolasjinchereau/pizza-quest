/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <cstdint>

#define PI					(3.141592654f)
#define HALFPI				(PI / 2.0f)
#define TWOPI				(PI * 2.0f)
#define RADIANS(x)			((float)(x) * (PI / 180.0f))
#define DEGREES(x)			((float)(x) * (180.0f / PI))

namespace math
{
	template<class T> inline T clamp(T x, T xMin, T xMax) {
		return x < xMin ? xMin : (x > xMax ? xMax : x);
	}

	template<int power> inline float pow(float x) { return ::pow(x, power); }
	template<> inline float pow<0>(float x)  { return 1; }
	template<> inline float pow<1>(float x)  { return x; }
	template<> inline float pow<2>(float x)  { return x*x; }
	template<> inline float pow<3>(float x)  { return x*x*x; }
	template<> inline float pow<4>(float x)  { return x*x*x*x; }
	template<> inline float pow<5>(float x)  { return x*x*x*x*x; }
	template<> inline float pow<6>(float x)  { return x*x*x*x*x*x; }
	template<> inline float pow<7>(float x)  { return x*x*x*x*x*x*x; }
	template<> inline float pow<8>(float x)  { return x*x*x*x*x*x*x*x; }
	template<> inline float pow<9>(float x)  { return x*x*x*x*x*x*x*x*x; }
	template<> inline float pow<10>(float x) { return x*x*x*x*x*x*x*x*x*x; }

	const static float pi = 3.141592654f;
	const static float twopi = 2.0f * 3.141592654f;

	inline float rad(float x) { return x * pi / 180.0f; }
	inline float deg(float x) { return x * 180.0f / pi; }

	template <typename T> inline T sign(T x) {
		return T((x > 0) - (0 > x));
	}

	template<class T> inline T lerp(const T &a, const T &b, float t) {
		return a + (b - a) * t;
	}

	template<typename T> T mod(T x, T y)
	{
		static_assert(!std::numeric_limits<T>::is_exact, "mod: floating-point type expected");

		if(y == 0)
			return x;

		T m = x - y * floor(x / y);

		// handle boundary cases resulted from floating-point cut off:

		if(y > 0) // modulo range: [0..y)
		{
			if(m >= y) // mod(-1e-16, 360.0) : m = 360.
				return 0;

			if(m < 0)
			{
				if(y + m == y)
					return 0; // just in case...
				else
					return y + m; // mod(106.81415022205296 , _TWO_PI ): m = -1.421e-14 
			}
		}
		else // modulo range: (y..0]
		{
			if(m <= y) // mod(1e-16, -360.0) : m = -360.
				return 0;

			if(m > 0)
			{
				if(y + m == y)
					return 0; // just in case...
				else
					return y + m; // mod(-106.81415022205296, -_TWO_PI): m = 1.421e-14 
			}
		}

		return m;
	}

	template<class T> inline float lerp_angle(T start, T end, T amount)
	{
		assert(start >= 0.0f && start < twopi);
		assert(end >= 0.0f && end < twopi);

		if(end > start + pi)
			start += twopi;
		else if(end < start - pi)
			end += twopi;
		
		return mod(start + (end - start) * amount, twopi);
	}
};

template<class T>
T Min4(const T &a, const T &b, const T &c, const T &d)
{
	T m = a;
	if(b < m) m = b;
	if(c < m) m = c;
	if(d < m) return d;
	return m;
}

template<class T>
T Max4(const T &a, const T &b, const T &c, const T &d)
{
	T m = a;
	if(b > m) m = b;
	if(c > m) m = c;
	if(d > m) return d;
	return m;
}

inline bool IsPowerOfTwo(unsigned int num)
{
	return (num != 0) && ((num & (num - 1)) == 0);
}

class Rect
{
public:
	float x, y, w, h;

	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(float X, float Y, float W, float H) : x(X), y(Y), w(W), h(H) {}

	void Set(float X, float Y, float W, float H);
	void Offset(float offset_x = 0, float offset_y = 0);
	void Deflate(float px = 0, float py = 0);
	bool Intersects(const Rect &rect) const;
	bool PointInside(float X, float Y) const;
};

class vec2f;
class vec3f;
class vec4f;

///////////////////////////////////////////////////////////////////////////////
//    mat3f
///////////////////////////////////////////////////////////////////////////////

class mat3f
{
public:
	
	float m11, m12, m13, //column 1
		  m21, m22, m23, //column 2
		  m31, m32, m33; //column 3

	mat3f()
	{
	}

	mat3f(bool loadIdentity)
	{
		if(loadIdentity) SetIdentity();
	}

	mat3f(float _11, float _21, float _31,
		 float _12, float _22, float _32,
		 float _13, float _23, float _33)
	   :m11(_11), m21(_21), m31(_31),
		m12(_12), m22(_22), m32(_32),
		m13(_13), m23(_23), m33(_33)
	{
	}
	
	void Set(float _11, float _21, float _31,
			 float _12, float _22, float _32,
			 float _13, float _23, float _33);

	vec2f operator*(const vec2f &V) const;
	vec3f operator*(const vec3f &V) const;
	mat3f operator*(const mat3f &M) const;
	mat3f &operator*=(const mat3f &M);
	void Translate(float x, float y);
	void SetOrtho2D(float left, float right, float bottom, float top);
	void SetIdentity();
	void SetTranslation(float x, float y);
	void SetScale(float sx, float sy);
	void SetRotation(float angle);
	void Transpose();
	static mat3f Scale(float sx = 1, float sy = 1, float sw = 1);
	static mat3f Translation(float x, float y);
	static mat3f Identity();
	static mat3f Ortho2D(float left, float right, float bottom, float top);
};

///////////////////////////////////////////////////////////////////////////////
//    mat4f
///////////////////////////////////////////////////////////////////////////////

class mat4f
{
public:

	float m11, m12, m13, m14, //column 1
		  m21, m22, m23, m24, //column 2
		  m31, m32, m33, m34, //column 3
		  m41, m42, m43, m44; //column 4
	
	mat4f(){}
	mat4f(bool loadIdentity)
	{
		if(loadIdentity) SetIdentity();
	}

	mat4f(float _11, float _21, float _31, float _41,
		 float _12, float _22, float _32, float _42,
		 float _13, float _23, float _33, float _43,
		 float _14, float _24, float _34, float _44)
		 :m11(_11), m21(_21), m31(_31), m41(_41),
		  m12(_12), m22(_22), m32(_32), m42(_42),
		  m13(_13), m23(_23), m33(_33), m43(_43),
		  m14(_14), m24(_24), m34(_34), m44(_44){}
	
	void Set(float _11, float _21, float _31, float _41,
			 float _12, float _22, float _32, float _42,
			 float _13, float _23, float _33, float _43,
			 float _14, float _24, float _34, float _44);

	vec4f operator*(const vec4f &V) const;
	mat4f operator*(const mat4f &M) const;
	mat4f operator*(float s) const;
	mat4f operator/(float s) const;

	mat4f &operator*=(const mat4f &M);
	mat4f &operator*=(float s);
	mat4f &operator/=(float s);

	float operator[](int i) const;

	void Translate(float x, float y, float z);
	void SetOrtho2D(float left, float right, float bottom, float top, float zNear, float zFar);
	void SetProject3D(float fov, float view_w, float view_h, float ZNear, float ZFar);
	void SetLookAt(const vec3f &eye, const vec3f &center, const vec3f &up);
	void SetIdentity();
	void SetTranslation(float x, float y, float z);
	void SetScale(float sx = 1, float sy = 1, float sz = 1, float sw = 1);
	void SetRotationX(float angle);
	void SetRotationY(float angle);
	void SetRotationZ(float angle);
	void Transpose();
	bool Invert();
	mat4f Inverse();

	static mat4f Scale(float sx = 1, float sy = 1, float sz = 1, float sw = 1);
	static mat4f Translation(float x, float y, float z);
	static mat4f Identity();
	static mat4f Ortho2D(float left, float right, float bottom, float top, float zNear, float zFar);
	static mat4f Project3D(float fov, float view_w, float view_h, float ZNear, float ZFar);
	static mat4f LookAt(const vec3f &eye, const vec3f &center, const vec3f &up);
};

///////////////////////////////////////////////////////////////////////////////
//    vec2f
///////////////////////////////////////////////////////////////////////////////

class vec2f
{
public:
	static const vec2f up;
	static const vec2f down;
	static const vec2f left;
	static const vec2f right;
	static const vec2f zero;
	static const vec2f one;

	float x, y;
	vec2f() : x(0), y(0){}
	vec2f(float _x, float _y) : x(_x), y(_y){}
	vec2f(const mat3f &mtx, const vec2f &vec);
	vec2f(const mat3f &mtx, float x, float y);

	void setX(float x_) { this->x = x_; }
	void setY(float y_) { this->y = y_; }
	void set(float x_, float y_) {x = x_; y = y_;}

	vec2f operator+(const vec2f &vec) const;
	vec2f operator-(const vec2f &vec) const;
	vec2f operator-() const;
	vec2f operator*(const float scalar) const;
	float operator*(const vec2f &vec) const;
	vec2f operator/(const float scalar) const;
	vec2f& operator+=(const vec2f &vec);
	vec2f& operator-=(const vec2f &vec);
	vec2f& operator*=(const float scalar);
	vec2f operator*(const mat3f &M) const;
	vec2f &operator*=(const mat3f &M);
	vec2f operator*(const mat4f &M) const;
	vec2f &operator*=(const mat4f &M);
	void LeftTransform(const mat3f &M, vec2f &V);
	void LeftTransform(const mat4f &M, vec2f &V);
	vec2f& operator/=(const float scalar);
	bool operator==(const vec2f &vec) const;
	bool operator!=(const vec2f &vec) const;
	vec2f PerpCW() const;
	vec2f PerpCCW() const;
	float Distance(const vec2f &vec) const;
	float DistanceSq(const vec2f &vec) const;
	float Length() const;
	float LengthSq() const;
	float Cross(const vec2f &vec) const;
	float Dot(const vec2f &other) const;
	void Normalize();
	vec2f Normalized() const;
	void Reverse();
	float Angle(const vec2f &other) const;
	vec2f Project(const vec2f &v) const;
	vec2f SignedProject(const vec2f &v) const;
};

///////////////////////////////////////////////////////////////////////////////
//    vec3f
///////////////////////////////////////////////////////////////////////////////

class vec3f
{
public:
	float x, y, z;
	
	static const vec3f zero;
	static const vec3f forward;
	static const vec3f up;
	static const vec3f right;

	vec3f() : x(0), y(0), z(0){}
	vec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}
	void set(float x_, float y_, float z_) {x = x_; y = y_; z = z_;}

	vec3f operator+(const vec3f &vec) const;
	vec3f operator-(const vec3f &vec) const;
	vec3f operator-() const;
	vec3f operator*(const float scalar) const;
	float operator*(const vec3f &vec) const;
	vec3f operator*(const mat4f &M) const;
	vec3f &operator*=(const mat4f &M);
	vec3f operator/(const float scalar) const;
	vec3f& operator+=(const vec3f &vec);
	vec3f& operator-=(const vec3f &vec);
	vec3f& operator*=(const float scalar);
	vec3f& operator/=(const float scalar);
	bool operator==(const vec3f &vec) const;
	bool operator!=(const vec3f &vec) const;
	float Dot(const vec3f &vec) const;
	vec3f Cross(const vec3f &vec) const;
	float Distance(const vec3f &vec) const;
	float DistanceSq(const vec3f &vec) const;
	float Length() const;
	float LengthSq() const;
	vec3f& Normalize();
	void Reverse();
};

///////////////////////////////////////////////////////////////////////////////
//    vec4f
///////////////////////////////////////////////////////////////////////////////

class vec4f
{
public:
	float x, y, z, w;

	static const vec4f zero;
	static const vec4f forward;
	static const vec4f up;
	static const vec4f right;

	vec4f() : x(0), y(0), z(0), w(1){}
	vec4f(float _x, float _y, float _z, float _w = 1) : x(_x), y(_y), z(_z), w(_w){}
	void set(float x_, float y_, float z_, float w_ = 1) {x = x_; y = y_; z = z_; w = w_;}

	vec4f operator+(const vec4f &vec) const;
	vec4f operator-(const vec4f &vec) const;
	vec4f operator*(const float scalar) const;
	float operator*(const vec4f &vec) const;
	vec4f operator*(const mat4f &M) const;
	vec4f &operator*=(const mat4f &M);
	void LeftTransform(const mat4f &M, const vec4f &V);
	vec4f operator/(const float scalar) const;
	vec4f& operator+=(const vec4f &vec);
	vec4f& operator-=(const vec4f &vec);
	vec4f& operator*=(const float scalar);
	vec4f& operator/=(const float scalar);
	bool operator==(const vec4f &vec) const;
	bool operator!=(const vec4f &vec) const;
	float Dot3(const vec4f &vec) const;
	float Dot4(const vec4f &vec) const;
	vec4f Cross(const vec4f &vec) const;
	float Distance(const vec4f &vec) const;
	float DistanceSq(const vec4f &vec) const;
	float Length() const;
	float LengthSq() const;
	vec4f& Normalize();
	void Reverse();
};

///////////////////////////////////////////////////////////////////////////////
//    Quaternion
///////////////////////////////////////////////////////////////////////////////

class Quaternion
{
public:
	static const Quaternion identity;

	vec3f v;
	float s;

	//! constructors
	Quaternion();
	Quaternion(float real, const vec3f &i);
	Quaternion(float real, float x, float y, float z);
	Quaternion(const vec3f &angles); //! from 3 euler angles 
	Quaternion(float theta_z, float theta_y, float theta_x); //! from 3 euler angles
	
	void Set(float x, float y, float z, float s);
	void Set(float theta_z, float theta_y, float theta_x);
	void SetIdentity();
	

	//! basic operations
	Quaternion &operator=(const Quaternion &q);
	const Quaternion operator+(const Quaternion &q) const;
	const Quaternion operator-(const Quaternion &q) const;
	const Quaternion operator*(const Quaternion &q) const;
	const Quaternion operator/(const Quaternion &q) const;
	const Quaternion operator*(float scale) const;
	const Quaternion operator/(float scale) const;
	const Quaternion operator-() const;
	const Quaternion &operator+=(const Quaternion &q);
	const Quaternion &operator-=(const Quaternion &q);
	const Quaternion &operator*=(const Quaternion &q);
	const Quaternion &operator*=(float scale);
	const Quaternion &operator/=(float scale);
	operator mat3f() const;
	operator mat4f() const;

	mat4f ToMatrix() const;
	float Length() const;
	float LengthSq() const;
	void Normalize();
	Quaternion Normalized() const;
	void Conjugate();
	void Invert();
	Quaternion Log() const;
	Quaternion Exp() const;
	static inline float Dot(const Quaternion &q1, const Quaternion &q2);
	static Quaternion Lerp(const Quaternion &q1, const Quaternion &q2, float t);
	static Quaternion Slerp(const Quaternion &q1, const Quaternion &q2, float t);
	static Quaternion SlerpNoInvert(const Quaternion &q1, const Quaternion &q2, float t);
	static Quaternion Squad(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t);
	static Quaternion Bezier(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t);
	static Quaternion Spline(const Quaternion &qnm1,const Quaternion &qn,const Quaternion &qnp1);
	static Quaternion FromAxisAngle(const vec3f &axis, float angle);
	void ToAxisAngle(vec3f &axis, float &angle) const;
	vec3f Rotate(const vec3f &v);
	vec3f Rotate(float x, float y, float z);
	vec3f EulerAngles(bool homogenous = true) const;
};

class Quad // drawable as a strip
{
public:
	vec2f TL; // top left
	vec2f BL; // bottom left
	vec2f TR; // top right
	vec2f BR; // bottom right
};

class Plane
{
public:
	Plane() : a(0), b(0), c(0), d(0){}

	float a, b, c, d;

	void Normalize()
	{
		float length = sqrtf(a * a + b * b + c * c);
		if(length > 0)
		{
			length = 1.0f / length;
			a *= length;
			b *= length;
			c *= length;
			d *= length;
		}
	}

	operator vec4f()
	{
		return vec4f(a, b, c, d);
	}
};

class Sphere
{
public:
	vec4f center;
	float radius;

	Sphere(){}
	Sphere(float x, float y, float z, float r)
	{
		center.set(x, y, z);
		radius = r;
	}

	void Set(float x, float y, float z, float r)
	{
		center.set(x, y, z);
		radius = r;
	}
};

class AABB
{
public:
	vec4f verts[8];

	AABB(const vec3f &vMin, const vec3f &vMax)
	{
		Set(vMin, vMax);
	}

	void Set(const vec3f &vMin, const vec3f &vMax)
	{
		// bottom square
		verts[0].set(vMin.x, vMin.y, vMin.z);
		verts[1].set(vMin.x, vMin.y, vMax.z);
		verts[2].set(vMax.x, vMin.y, vMax.z);
		verts[3].set(vMax.x, vMin.y, vMin.z);

		// top square
		verts[4].set(vMin.x, vMax.y, vMin.z);
		verts[5].set(vMin.x, vMax.y, vMax.z);
		verts[6].set(vMax.x, vMax.y, vMax.z);
		verts[7].set(vMax.x, vMax.y, vMin.z);
	}
};

class Ray
{
public:
	vec3f origin;
	vec3f direction;

	Ray(){}
	Ray(const vec3f &orig, const vec3f &dir)
		: origin(orig), direction(dir){}

	bool RayCast(const Sphere &s)
	{
		vec3f sp;
		sp.x = s.center.x;
		sp.y = s.center.y;
		sp.z = s.center.z;
		
		float t = (sp - origin).Dot(direction) / direction.Dot(direction);

		if(t < 0.0f)
			return false;
		
		vec3f worldPt = origin + (direction * t);

		return sp.DistanceSq( worldPt ) <= (s.radius * s.radius);
	}
};

class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	static Color red;
	static Color green;
	static Color blue;
	static Color yellow;
	static Color orange;
	static Color purple;
	static Color magenta;
	static Color cyan;
	static Color black;
	static Color white;
	static Color gray;
	static Color clear;

	Color() : r(0), g(0), b(0), a(0){}
	
	Color(float r, float g, float b, float a = 1.0f)
		: r(r), g(g), b(b), a(a){}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r(r  / 255.0f), g(g  / 255.0f), b(b  / 255.0f), a(a  / 255.0f){}

	Color(const vec4f &other)
		: r(other.x), g(other.y), b(other.z), a(other.w){}

	Color(uint32_t color32)
		: r(((color32 & 0xFF000000) >> 24) / 255.0f),
		  g(((color32 & 0xFF0000) >> 16) / 255.0f),
		  b(((color32 & 0xFF00) >> 8) / 255.0f),
		  a((color32 & 0xFF) / 255.0f){}

	Color &operator=(const vec4f &other);
	operator uint32_t();
	operator vec4f&();
	operator float*();

	static uint32_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};

struct Random
{
	static float value();
	static float signedValue();
	static vec2f vector2();
	static float range(float min, float max);
};
