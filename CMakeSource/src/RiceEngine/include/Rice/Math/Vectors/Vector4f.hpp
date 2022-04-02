struct Vector4f;

#pragma once

#include "../../defines.h"


struct Vector4f {
	float x = 0, y = 0, z = 0, w = 0;
	Vector4f() = default;
	Vector4f(float x, float y, float z, float w = 1.f);
	//Vector4f(const D2D1::ColorF& color);
};
