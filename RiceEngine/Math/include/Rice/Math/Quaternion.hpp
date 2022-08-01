struct Quaternion;

#pragma once

#include "stdafx.hpp"
#include "Vectors/Vector3.hpp"

struct Quaternion {
	dbl x, y, z, w;

	static Quaternion fromAxisAngle(Vector3 axis) { return fromAxisAngle(axis.Normalized(), axis.Length()); };
	static Quaternion fromAxisAngle(Vector3 axis, double angle);
	static Quaternion lookAt(Vector3 sourcePoint, Vector3 destPoint);
	static Quaternion fromEulerAngles(Vector3 v) { return fromEulerAngles(v.x, v.y, v.z); }
	static Quaternion fromEulerAngles(double x, double y, double z);

	dbl magnitude();

	Quaternion operator-();
	Quaternion inversed();
	Quaternion opposite();
	Quaternion normalized();

	Vector3 toEulerAngles();
	Vector3 toAxisAngle();

	const static Quaternion identity;
};

Quaternion operator*(Quaternion A, Quaternion b);

Quaternion operator*(Quaternion A, dbl b);
