struct Vector3f;

#pragma once

#include "../../defines.h"
#include "../Quaternion.hpp"

struct Vector3f {
public:
	float x = 0, y = 0, z = 0;
	Vector3f() = default;
	Vector3f(float x, float y) : x(x), y(y) {}
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3f(const Vector3& vec);

	Vector3f& operator=(Vector3& v);
	Vector3f& operator-=(Vector3f v);
	Vector3f& operator+=(Vector3f v);
	Vector3f& operator*=(float v);
	Vector3f& operator/=(float v);
	float& operator[](size_t idx);
	Vector3f operator-();

	Vector3f Normalized();

	void Normalize();
	void Qnormalize();

	static dbl Dot(Vector3f a, Vector3f b);
	static Vector3f Cross(Vector3f a, Vector3f b);
	static Vector3f Lerp(Vector3f a, Vector3f b, float t);
	dbl SqrLength();
	dbl Length();
};

bool operator==(Vector3f a, Vector3f b);

Vector3f operator+(Vector3f a, Vector3f b);
Vector3f operator-(Vector3f a, Vector3f b);
Vector3f operator*(Vector3f a, float b);
Vector3f operator/(Vector3f a, float b);
