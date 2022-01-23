#pragma once

#include "../stdafx.h"

struct Vector4f {
	float x, y, z, w;
	Vector4f() = default;
	Vector4f(float x, float y, float z, float w = 1.f);
	Vector4f(const D2D1::ColorF& color);
};