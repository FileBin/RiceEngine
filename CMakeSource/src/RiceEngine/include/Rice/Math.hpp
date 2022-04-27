﻿#pragma once
#include "stdafx.hpp"
#include "defines.h"
#include "Math/Quaternion.hpp"
#include "Math/Vectors.hpp"
#include "Math/Matrixes.hpp"

#include <math.h>
#include <cmath>

NSP_ENGINE_BEGIN

class Math {
public:
	static double abs(double x) {
		return abs(x);
	}

	static Vector3 GetNorm(std::function<double(Vector3)> func, Vector3 pos,
			dbl eps = .00001);

	static double asin(double x) {
		return toDegrees(asin(x));
	}
	static double acos(double x) {
		return toDegrees(acos(x));
	}
	static double atan(double x) {
		return toDegrees(atan(x));
	}
	static double atan2(double y, double x) {
		return toDegrees(atan2(y, x));
	}

	static double sin(double x) {
		return sin(toRadians(x));
	}
	static double cos(double x) {
		return cos(toRadians(x));
	}
	static double tan(double x) {
		return tan(toRadians(x));
	}

	static double toRadians(double x) {
		return x / 180. * PI;
	}
	static double toDegrees(double x) {
		return x * 180. / PI;
	}

	static dbl Random(double _min = 0., double _max = 1.) {
		return ((dbl) rand() / SHRT_MAX) * (_max - _min) + _min;
	}

	static dbl Qrsqrt(dbl x) {
		dbl xhalf = 0.5 * x;
		num i = *(num*) &x;				   //evil bit hack
		i = 0x5fe6ec85e7de30da - (i >> 1); //what a fuck?
		x = *(dbl*) &i;

		x *= (1.5 - xhalf * x * x); //1st iteration
		x *= (1.5 - xhalf * x * x); //2nd iteration
		//x *= (1.5 - xhalf * x * x); //3rd iteration
		//x *= (1.5 - xhalf * x * x); //4th iteration

		return x;
	}

	static float Qrsqrt(float x) {
		float xhalf = 0.5 * x;
		long i = *(long*) &x;	   //evil bit hack
		i = 0x5f3759df - (i >> 1); //what a fuck?
		x = *(float*) &i;

		x *= (1.5f - xhalf * x * x); //1st iteration
		x *= (1.5f - xhalf * x * x); //2nd iteration
		//x *= (1.5 - xhalf * x * x); //3rd iteration
		//x *= (1.5 - xhalf * x * x); //4th iteration

		return x;
	}

	template<typename T>
	static T Max(T a, T b) {
		return max(a, b);
	}

	template<typename T>
	static T Clamp(T v, T a, T b) {
		return min(max(v, a), b);
	}

	template<typename T>
	static T Clamp01(T v) {
		return Clamp<T>(v, 0, 1);
	}

	template<typename T>
	static T Min(T a, T b) {
		return min(a, b);
	}

	template<typename T>
	static T Lerp(T a, T b, T t) {
		return a * (1 - t) + b * t;
	}

	constexpr static double PI = 3.14159265359;
};

NSP_ENGINE_END