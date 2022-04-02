struct Vector2i;

#pragma once

#include "../../defines.h"

struct Vector2i {
	num x = 0, y = 0;
	Vector2i() {}
	Vector2i(num x, num y) : x(x), y(y) {}

	Vector2i& operator-=(Vector2i v);
	Vector2i& operator+=(Vector2i v);
	Vector2i& operator*=(num v);
	Vector2i& operator/=(num v);
	num& operator[](size_t idx);
	Vector2i operator-();

	static dbl Dot(Vector2i a, Vector2i b);
	dbl SqrLength();
	dbl Length();
};

bool operator==(Vector2i a, Vector2i b);

Vector2i operator+(Vector2i a, Vector2i b);
Vector2i operator-(Vector2i a, Vector2i b);
Vector2i operator*(Vector2i a, num b);
Vector2i operator/(Vector2i a, num b);
