#pragma once
#include "../Util/defines.h"

struct Vector3i {
public:
	num x, y, z;
	Vector3i() = default;
	Vector3i(num x, num y, num z) : x(x), y(y), z(z) {}
	Vector3i(const Vector3& vec);
	num SqrLength();
	
	static num Dot(Vector3i a, Vector3i b);
	static Vector3i Cross(Vector3i a, Vector3i b);

	num& operator[](size_t idx);
	Vector3i operator-();
};

bool operator==(Vector3i a, Vector3i b);

Vector3i operator+(Vector3i a, Vector3i b);
Vector3i operator-(Vector3i a, Vector3i b);
Vector3i operator*(Vector3i a, num b);
Vector3i operator/(Vector3i a, num b);

