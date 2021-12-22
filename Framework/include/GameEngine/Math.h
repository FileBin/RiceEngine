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

		const static double PI;
	};
}