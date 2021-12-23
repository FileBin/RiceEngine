﻿#pragma once

#include "Util.h"

struct Quaternion;
struct Vector3f;
struct Vector3i;

#include "Vectors/Vector2.h"

#include "Vectors/Vector3.h"

#include "Vectors/Vector3i.h"

struct Vector2f {
	float x, y;
};

struct Vector3f {
	float x, y, z;
	Vector3f& operator=(Vector3& v);
	Vector3f& operator+=(Vector3f& v);
};

struct Vector4f {
	float x, y, z, w;
};

Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, dbl b);
Vector2 operator/(Vector2 a, dbl b);

