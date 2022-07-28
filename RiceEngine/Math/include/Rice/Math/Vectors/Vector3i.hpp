struct Vector3i;

#pragma once

#include "../stdafx.hpp"

struct Vector3i {
public:
	num x = 0, y = 0, z = 0;
	Vector3i() = default;
	Vector3i(num x, num y) : x(x), y(y) {}
	Vector3i(num x, num y, num z) : x(x), y(y), z(z) {}
	Vector3i(const Vector3& vec);

	Vector3i& operator=(Vector3& v);
	Vector3i& operator-=(Vector3i v);
	Vector3i& operator+=(Vector3i v);
	Vector3i& operator*=(num v);
	Vector3i& operator/=(num v);
	num& operator[](size_t idx);
	Vector3i operator-();

	static num Dot(Vector3i a, Vector3i b);
	static Vector3i Cross(Vector3i a, Vector3i b);
	dbl SqrLength();
	dbl Length();

	static const Vector3i zero;
	static const Vector3i one;
	static const Vector3i left;
	static const Vector3i right;
	static const Vector3i up;
	static const Vector3i down;
	static const Vector3i forward;
	static const Vector3i backward;
};

bool operator==(Vector3i a, Vector3i b);

Vector3i operator+(Vector3i a, Vector3i b);
Vector3i operator-(Vector3i a, Vector3i b);
Vector3i operator*(Vector3i a, num b);
Vector3i operator/(Vector3i a, num b);
