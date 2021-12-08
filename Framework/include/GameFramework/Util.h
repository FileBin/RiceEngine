#pragma once
#include "stdafx.h"

namespace GameEngine {
	void ThrowIfFailed(HRESULT hr);

	struct Color {
		float r, g, b, a;
		Color(float red, float green, float blue) : r(red), g(green), b(blue), a(1) {}
		Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}
	};
}