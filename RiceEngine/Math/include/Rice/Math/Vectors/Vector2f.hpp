struct Vector2f;

#pragma once

#include "../stdafx.hpp"

struct Vector2f {
	float x = 0, y = 0;
	Vector2f() = default;
	Vector2f(float x, float y) : x(x), y(y) {}
	Vector2f(const Vector2& vec) : x(vec.x), y(vec.y) {}

	Vector2f& operator-=(Vector2f v);
	Vector2f& operator+=(Vector2f v);
	Vector2f& operator*=(float v);
	Vector2f& operator/=(float v);
	float& operator[](size_t idx);
	Vector2f operator-();

	static dbl Dot(Vector2f a, Vector2f b);
	dbl SqrLength();
	dbl Length();
};

bool operator==(Vector2f a, Vector2f b);

Vector2f operator+(Vector2f a, Vector2f b);
Vector2f operator-(Vector2f a, Vector2f b);
Vector2f operator*(Vector2f a, float b);
Vector2f operator/(Vector2f a, float b);
