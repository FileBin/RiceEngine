#pragma once

#include "../Util/defines.h"

struct Vector2 {
	Vector2() {}
	Vector2(dbl x, dbl y) : x(x), y(y) {}
	dbl x = 0, y = 0;
	static dbl Dot(Vector2 A, Vector2 b);

	Vector2 operator-=(Vector2 v);
	Vector2 operator+=(Vector2 v);
	Vector2 operator*=(dbl v);
	Vector2 operator/=(dbl v);

	dbl Length();
	dbl SqrLength() { return x * x + y * y; }

	static const Vector2 zero;
	static const Vector2 one;
	static const Vector2 left;
	static const Vector2 right;
	static const Vector2 up;
	static const Vector2 down;
};