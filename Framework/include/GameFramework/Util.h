#pragma once
#include "stdafx.h"

namespace GameEngine::Util {
	void ThrowIfFailed(HRESULT hr);
	data_t ReadFile(String filename);
	String Utf8ToWstring(const std::string& str);

	struct Color {
		float r, g, b, a;
		Color(float red, float green, float blue) : r(red), g(green), b(blue), a(1) {}
		Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}
	};
}