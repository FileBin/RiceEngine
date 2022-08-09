#pragma once
#include "stdafx.hpp"
#include "Quaternion.hpp"
#include "Vectors.hpp"
#include "Matrixes.hpp"

NSP_ENGINE_BEGIN

template<typename T>
class MathT;

template<typename T>
class MathT {
public:
	static double abs(T x) {
		return std::abs(x);
	}

	static Vector3 getNorm(std::function<T(Vector3)> func, Vector3 pos,
			dbl eps = .00001);

	static T asin(T x) {
		return toDegrees(std::asin(x));
	}
	static T acos(T x) {
		return toDegrees(std::acos(x));
	}
	static T atan(T x) {
		return toDegrees(std::atan(x));
	}

	static T atan(T y, T x) {
		return toDegrees(std::atan2(y, x));
	}

	static T sin(T x) {
		return std::sin(toRadians(x));
	}
	static T cos(T x) {
		return std::cos(toRadians(x));
	}
	static T tan(T x) {
		return std::tan(toRadians(x));
	}

	static T toRadians(T x) {
		return x / 180. * PI;
	}
	static T toDegrees(T x) {
		return x * 180. / PI;
	}

	static T random(T _min = 0., T _max = 1.) {
		return ((dbl) rand() / RAND_MAX) * (_max - _min) + _min;
	}

	static dbl qrsqrt(dbl x) {
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

	static float qrsqrt(float x) {
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

	static T max(T a, T b) {
		return a > b ? a : b;
	}

	static T min(T a, T b) {
		return a < b ? a : b;
	}

	static T clamp(T v, T a, T b) {
		return min(max(v, a), b);
	}

	static T clamp01(T v) {
		return clamp(v, 0, 1);
	}

	static T lerp(T a, T b, T t) {
		return a * (1 - t) + b * t;
	}

	constexpr static double PI = 3.14159265359;
};

typedef MathT<dbl> Math;
typedef MathT<float> Mathf;

NSP_ENGINE_END
