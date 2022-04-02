struct Vector3;

#pragma once
#include "../../defines.h"

struct Vector3i;
struct Quaternion;
struct Vector3f;

struct Vector3 {
public:
	dbl x = 0, y = 0, z = 0;
	Vector3() = default;
	Vector3(dbl x, dbl y) : x(x), y(y) {}
	Vector3(dbl x, dbl y, dbl z) : x(x), y(y), z(z) {}
	Vector3(const Vector3i& vec);
	Vector3(const Vector3f& vec);

	Vector3(const Vector2& vec);

	Vector3& operator=(Vector3f& v);
	Vector3& operator-=(Vector3 v);
	Vector3& operator+=(Vector3 v);
	Vector3& operator*=(dbl v);
	Vector3& operator/=(dbl v);
	Vector3 operator*=(Quaternion q);
	dbl& operator[](size_t idx);
	Vector3 operator-() const;

	Vector3 Normalized();

	void Normalize();
	void Qnormalize();

	static dbl Dot(Vector3 a, Vector3 b);
	static Vector3 cross(Vector3 a, Vector3 b);
	static Vector3 Lerp(Vector3 a, Vector3 b, dbl t);
	static Vector3 ProjectOnPlane(Vector3 vec, Vector3 planeNormal);
	dbl SqrLength();
	dbl Length();

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 left;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 forward;
	static const Vector3 backward;
};

bool operator==(Vector3 a, Vector3 b);

Vector3 operator+(Vector3 a, Vector3 b);
Vector3 operator-(Vector3 a, Vector3 b);
Vector3 operator*(Vector3 a, dbl b);
Vector3 operator/(Vector3 a, dbl b);
Vector3 operator/(dbl a, Vector3 b);
Vector3 operator*(Quaternion q, Vector3 v);
