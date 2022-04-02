struct Quaternion;

#pragma once

#include "../defines.h"
#include "Vectors/Vector3.hpp"

struct Quaternion {
	dbl x, y, z, w;

	static Quaternion FromAxisAngle(Vector3 axis) { return fromAxisAngle(axis.Normalized(), axis.Length()); };
	static Quaternion fromAxisAngle(Vector3 axis, double angle);
	static Quaternion LookAt(Vector3 sourcePoint, Vector3 destPoint);
	static Quaternion FromEulerAngles(Vector3 v) { return fromEulerAngles(v.x, v.y, v.z); }
	static Quaternion fromEulerAngles(double x, double y, double z);

	dbl Magnitude();

	Quaternion operator-();
	Quaternion Inversed();
	Quaternion Opposite();
	Quaternion Normalized();

	Vector3 ToEulerAngles();
	Vector3 ToAxisAngle();

	const static Quaternion identity;
};

Quaternion operator*(Quaternion A, Quaternion b);

Quaternion operator*(Quaternion A, dbl b);
