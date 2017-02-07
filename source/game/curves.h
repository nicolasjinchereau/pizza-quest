/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Math.h"

namespace crv
{
	typedef float(*function_t)(float t);

	struct normal_limiter
	{
		inline float low() { return 0.0f; }
		inline float height() { return 1.0f; }
	};

	template<function_t CURVE>
	inline float inv(float t)
	{
		return 1.0f - CURVE(t);
	}

	template<function_t CURVE_IN, function_t CURVE_OUT>
	inline float inout(float t)
	{
		return t < 0.5f ? CURVE_IN(t * 2.0f) : CURVE_OUT(2.0f - 2.0f * t);
	}

	template<function_t CURVE>
	inline float inout(float t)
	{
		return t < 0.5f ? CURVE(t * 2.0f) : CURVE(2.0f - 2.0f * t);
	}

	template<function_t CURVE, class LIMITER> // = normal_limiter
	inline float limit(float t)
	{
		LIMITER limiter;
		float _low = limiter.low();
		float _high = limiter.high();
		return _low + CURVE(t) * (_high - _low);
	}
	
	inline float constant(float t)
	{
		return 1.0f;
	}

	inline float in_linear(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t;
	}

	inline float out_linear(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1.0f - t;
	}

	inline float in_quad(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t * t;
	}

	inline float in_quad_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1 - (t * t);
	}

	inline float out_quad(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		t = (1 - t);
		return t * t;
	}

	inline float out_quad_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t * (2 - t);
	}

	inline float in_cube(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t * t * t;
	}

	inline float in_cube_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1 - (t * t * t);
	}

	inline float out_cube(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float inv = 1 - t;
		return inv * inv * inv;
	}

	inline float out_cube_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float inv = 1 - t;
		return 1 - (inv * inv * inv);
	}

	inline float in_dec(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // t ^ 2
		float t4 = t2 * t2; // t ^ 4
		return t2 * t4 * t4;
	}

	inline float in_dec_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // t ^ 2
		float t4 = t2 * t2; // t ^ 4
		return 1 - t2 * t4 * t4;
	}

	inline float out_dec(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = 1 - t;
		float t2 = t_inv * t_inv; // t_inv ^ 2
		float t4 = t2 * t2; // t_inv ^ 4
		return t2 * t4 * t4;
	}

	inline float out_dec_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = 1 - t;
		float t2 = t_inv * t_inv; // t_inv ^ 2
		float t4 = t2 * t2; // t_inv ^ 4
		return 1 - t2 * t4 * t4;
	}

	inline float in_icosa(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // t ^ 2
		float t4 = t2 * t2; // t ^ 4
		float t8 = t4 * t4; // t ^ 8
		return t8 * t8 * t4; // t ^ 20
	}

	inline float in_icosa_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // t ^ 2
		float t4 = t2 * t2; // t ^ 4
		float t8 = t4 * t4; // t ^ 8
		return 1- t8 * t8 * t4; // 1 - t ^ 20
	}

	inline float out_icosa(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = (1 - t);
		float t_inv2 = t_inv * t_inv; // (1 - t) ^ 2
		float t_inv4 = t_inv2 * t_inv2; // (1 - t) ^ 4
		float t_inv8 = t_inv4 * t_inv4; // (1 - t) ^ 8
		return t_inv8 * t_inv8 * t_inv4; // (1 - t) ^ 20
	}

	inline float out_icosa_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = (1 - t);
		float t_inv2 = t_inv * t_inv; // (1 - t) ^ 2
		float t_inv4 = t_inv2 * t_inv2; // (1 - t) ^ 4
		float t_inv8 = t_inv4 * t_inv4; // (1 - t) ^ 8
		return 1 - t_inv8 * t_inv8 * t_inv4; // (1 - t) ^ 20
	}

	inline float in_tetraconta(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // (1 - t) ^ 2
		float t4 = t2 * t2; // (1 - t) ^ 4
		float t8 = t4 * t4; // (1 - t) ^ 8
		float t16 = t8 * t8; // (1 - t) ^ 16
		return t16 * t16 * t8; // (1 - t) ^ 40
	}

	inline float in_tetraconta_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t2 = t * t; // (1 - t) ^ 2
		float t4 = t2 * t2; // (1 - t) ^ 4
		float t8 = t4 * t4; // (1 - t) ^ 8
		float t16 = t8 * t8; // (1 - t) ^ 16
		return 1 - t16 * t16 * t8; // (1 - t) ^ 40
	}

	inline float out_tetraconta(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = (1 - t);
		float t_inv2 = t_inv * t_inv; // (1 - t) ^ 2
		float t_inv4 = t_inv2 * t_inv2; // (1 - t) ^ 4
		float t_inv8 = t_inv4 * t_inv4; // (1 - t) ^ 8
		float t_inv16 = t_inv8 * t_inv8; // (1 - t) ^ 16
		return t_inv16 * t_inv16 * t_inv8; // (1 - t) ^ 40
	}

	inline float out_tetraconta_inv(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t_inv = (1 - t);
		float t_inv2 = t_inv * t_inv; // (1 - t) ^ 2
		float t_inv4 = t_inv2 * t_inv2; // (1 - t) ^ 4
		float t_inv8 = t_inv4 * t_inv4; // (1 - t) ^ 8
		float t_inv16 = t_inv8 * t_inv8; // (1 - t) ^ 16
		return 1 - t_inv16 * t_inv16 * t_inv8; // (1 - t) ^ 40
	}

	inline float smoothstep_in(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t * t * (3 - (2 * t));
	}
	
	inline float smoothstep_out(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1 - (t * t * (3 - (2 * t)));
	}

	inline float smoothstep_inout(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return (t < 0.5f) ? (t * t * (3 - (2 * t))) : (1 - (t * t * (3 - (2 * t))));
	}

	inline float smoothstep_in_steep(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return t * t * t * (t * (6 * t - 15) + 10);
	}

	inline float smoothstep_out_steep(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1 - (t * t * t * (t * (6 * t - 15) + 10));
	}

	inline float smoothstep_inout_steep(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return (t < 0.5f) ? (t * t * t * (t * (6 * t - 15) + 10)) : (1 - (t * t * t * (t * (6 * t - 15) + 10)));
	}

	inline float in_elastic(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float ts = t * t;
		float tc = t * t * t;
		return 33 * tc * ts - 106 * ts * ts + 126 * tc - 67 * ts + 15 * t;
	}

	inline float in_elastic_strong(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float ts = t * t;
		float tc = t * t * t;
		return 56 * tc * ts - 175 * ts * ts + 200 * tc - 100 * ts + 20 * t;
	}

	inline float out_elastic(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float ts = t * t;
		float tc = t * t * t;
		return 1 - (33 * tc * ts - 106 * ts * ts + 126 * tc - 67 * ts + 15 * t);
	}

	inline float out_elastic_strong(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float ts = t * t;
		float tc = t * t * t;
		return 1 - (56 * tc * ts - 175 * ts * ts + 200 * tc - 100 * ts + 20 * t);
	}

	inline float arc_linear(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return 1 - (2 * abs(t - 0.5f));
	}

	inline float inv_arc_linear(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		return (2 * abs(t - 0.5f));
	}

	inline float arc_quad(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		return 1 - (x * x);
	}

	inline float inv_arc_quad(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		return (x * x);
	}

	inline float arc_cube(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		return 1 - ((0.5f * xsq) + (0.5f * (xsq * xsq)));
	}

	inline float inv_arc_cube(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		return (0.5f * xsq) + (0.5f * (xsq * xsq));
	}

	inline float arc_quart(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		return 1 - (xsq * xsq);
	}

	inline float inv_arc_quart(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		return (xsq * xsq);
	}

	inline float arc_oct(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		float xc = xsq * xsq;
		return 1 - (xc * xc);
	}

	inline float inv_arc_oct(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t - 1);
		float xsq = x * x;
		float xc = xsq * xsq;
		return (xc * xc);
	}

	inline float arc_quad_out_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t1 = t - 1;
		float x = (2 * t1 * t1 - 1);
		return 1 - (x * x);
	}

	inline float arc_quad_in_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t * t - 1);
		return 1 - (x * x);
	}

	inline float arc_cube_out_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t1 = t - 1;
		float x = (2 * t1 * t1 - 1);
		float xsq = x * x;
		return 1 - ((0.5f * xsq) + (0.5f * (xsq * xsq)));
	}

	inline float arc_cube_in_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t * t - 1);
		float xsq = x * x;
		return 1 - ((0.5f * xsq) + (0.5f * (xsq * xsq)));
	}

	inline float arc_quart_out_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float t1 = t - 1;
		float x = (2 * t1 * t1 - 1);
		float xsq = x * x;
		return 1 - (xsq * xsq);
	}

	inline float arc_quart_in_sharp(float t)
	{
		t = math::clamp(t, 0.0f, 1.0f);
		float x = (2 * t * t - 1);
		float xsq = x * x;
		return 1 - (xsq * xsq);
	}
}
