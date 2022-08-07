#pragma once
#include "stdafx.hpp"
#include "Quaternion.hpp"
#include "Vectors.hpp"
#include "Matrixes.hpp"

#include <cstdlib>
#include <math.h>
#include <cmath>

NSP_ENGINE_BEGIN

class Math {
public:
	static double abs(double x) {
		return std::abs(x);
	}

	static Vector3 getNorm(std::function<double(Vector3)> func, Vector3 pos,
			dbl eps = .00001);

	static double asin(double x) {
		return toDegrees(std::asin(x));
	}
	static double acos(double x) {
		return toDegrees(std::acos(x));
	}
	static double atan(double x) {
		return toDegrees(std::atan(x));
	}

	static double atan(double y, double x) {
		return toDegrees(std::atan2(y, x));
	}

	static double sin(double x) {
		return std::sin(toRadians(x));
	}
	static double cos(double x) {
		return std::cos(toRadians(x));
	}
	static double tan(double x) {
		return std::tan(toRadians(x));
	}

	static double toRadians(double x) {
		return x / 180. * PI;
	}
	static double toDegrees(double x) {
		return x * 180. / PI;
	}

	static dbl random(double _min = 0., double _max = 1.) {
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

	template<typename T>
	static T max(T a, T b) {
		return a > b ? a : b;
	}

	template<typename T>
	static T min(T a, T b) {
		return a < b ? a : b;
	}

	template<typename T>
	static T clamp(T v, T a, T b) {
		return min(max(v, a), b);
	}

	template<typename T>
	static T clamp01(T v) {
		return clamp<T>(v, 0, 1);
	}

	template<typename T>
	static T lerp(T a, T b, T t) {
		return a * (1 - t) + b * t;
	}

	constexpr static double PI = 3.14159265359;
};

NSP_ENGINE_END
