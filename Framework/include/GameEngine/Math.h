#pragma once
#include "Util/defines.h"
#include <cmath>

namespace Game
{
	class Math
	{
	public:
		static double Abs(double x) { return abs(x); }

		static double Asin(double x) { return ToDegrees(asin(x)); }
		static double Acos(double x) { return ToDegrees(acos(x)); }
		static double Atan(double x) { return ToDegrees(atan(x)); }
		static double Atan2(double y, double x) { return ToDegrees(atan2(y, x)); }

		static double Sin(double x) { return sin(ToRadians(x)); }
		static double Cos(double x) { return cos(ToRadians(x)); }
		static double Tan(double x) { return tan(ToRadians(x)); }

		static double ToRadians(double x) { return x / 180. * PI; }
		static double ToDegrees(double x) { return x * 180. / PI; }

		static dbl Random(double _min = 0., double _max = 1.) {
			return ((dbl)rand() / SHRT_MAX)*(_max - _min) + _min;
		}

		static dbl Qrsqrt(dbl x) {
			dbl xhalf = 0.5 * x;
			num i = *(num*)&x;				   //evil bit hack
			i = 0x5fe6ec85e7de30da - (i >> 1); //what a fuck?
			x = *(dbl*)&i;

			x *= (1.5 - xhalf * x * x); //1st iteration
			x *= (1.5 - xhalf * x * x); //2nd iteration
			//x *= (1.5 - xhalf * x * x); //3rd iteration
			//x *= (1.5 - xhalf * x * x); //4th iteration

			return x;
		}

		static float Qrsqrt(float x) {
			float xhalf = 0.5 * x;
			long i = *(long*)&x;	   //evil bit hack
			i = 0x5f3759df - (i >> 1); //what a fuck?
			x = *(float*)&i;

			x *= (1.5f - xhalf * x * x); //1st iteration
			x *= (1.5f - xhalf * x * x); //2nd iteration
			//x *= (1.5 - xhalf * x * x); //3rd iteration
			//x *= (1.5 - xhalf * x * x); //4th iteration

			return x;
		}

		template<typename T>
		static T Max(T a, T b) { return max(a, b); }

		template<typename T>
		static T Clamp(T v, T a, T b) { return min(max(v, a), b); }

		template<typename T>
		static T Clamp01(T v) { return Clamp<T>(v, 0, 1); }

		template<typename T>
		static T Min(T a, T b) { return min(a, b); }

		template<typename T>
		static T Lerp(T a, T b,T t) { return a*(1 - t) + b*t; }

		const static double PI;
	};
}