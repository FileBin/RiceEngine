#pragma once
#include "Vectors.h"
#include <cmath>

namespace Game {
	class Math {
	public:
		static double Abs(double x) { return abs(x); }

		static double Asin(double x) { return ToDegrees(asin(x)); }
		static double Acos(double x) { return ToDegrees(acos(x)); }
		static double Atan(double x) { return ToDegrees(atan(x)); }
		static double Atan2(double y, double x) { return ToDegrees(atan2(y,x)); }

		static double Sin(double x) { return sin(ToRadians(x)); }
		static double Cos(double x) { return cos(ToRadians(x)); }
		static double Tan(double x) { return tan(ToRadians(x)); }

		static double ToRadians(double x) { return x / 180. * PI; }
		static double ToDegrees(double x) { return x * 180. / PI; }

		const static double PI;
	};
}