﻿struct Vector2;

#pragma once

#include "../../defines.h"

#include "Vector2i.hpp"

struct Vector2 {
	dbl x = 0, y = 0;
	Vector2() {}
	Vector2(const Vector2i&);
	Vector2(dbl x, dbl y) : x(x), y(y) {}

	Vector2& operator-=(Vector2 v);
	Vector2& operator+=(Vector2 v);
	Vector2& operator*=(dbl v);
	Vector2& operator/=(dbl v);
	Vector2& operator/=(Vector2 v);
	dbl& operator[](size_t idx);
	Vector2 operator-();

	static dbl Dot(Vector2 a, Vector2 b);
	dbl SqrLength();
	dbl Length();
	Vector2 Normalized();

	static const Vector2 zero;
	static const Vector2 one;
	static const Vector2 left;
	static const Vector2 right;
	static const Vector2 up;
	static const Vector2 down;
};

bool operator==(Vector2 a, Vector2 b);

Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, dbl b);
Vector2 operator/(Vector2 a, dbl b);
Vector2 operator*(Vector2 a, Vector2 b);
Vector2 operator/(Vector2 a, Vector2 b);