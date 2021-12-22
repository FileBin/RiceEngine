#pragma once

#include "../Util/defines.h"

struct Vector3;

struct Quaternion {
	double x, y, z, w;

	static Quaternion FromAxisAngle(Vector3 axis) { return FromAxisAngle(axis.Normalized(), axis.Length()); };
	static Quaternion FromAxisAngle(Vector3 axis, double angle);
	static Quaternion LookAt(Vector3 sourcePoint, Vector3 destPoint);
	static Quaternion FromEulerAngles(Vector3 v) { return FromEulerAngles(v.x, v.y, v.z); }
	static Quaternion FromEulerAngles(double x, double y, double z);

	double Magnitude();

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