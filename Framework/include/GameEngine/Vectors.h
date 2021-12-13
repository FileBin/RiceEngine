#pragma once

struct Vector2 {
	Vector2() {}
	Vector2(double x, double y) : x(x), y(y) {}
	double x = 0, y = 0;
	static double Dot(Vector2 a, Vector2 b);

	static const Vector2 zero;
};

struct Vector3 {
	Vector3() {}
	Vector3(double x, double y) : x(x), y(y) {}
	Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
	double x = 0, y = 0, z = 0;

	double Length();
	double SqrLength();

	Vector3 Normalized();

	static double Dot(Vector3 a, Vector3 b);
	static Vector3 Cross(Vector3 a, Vector3 b);

	Vector3 operator-();

	static const Vector3 zero;
	static const Vector3 left;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 forward;
	static const Vector3 backward;
};

struct Vector3f {
	float x, y, z;
};

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

Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, double b);
Vector2 operator/(Vector2 a, double b);
Vector3 operator+(Vector3 a, Vector3 b);
Vector3 operator-(Vector3 a, Vector3 b);
Vector3 operator*(Vector3 a, double b);
Vector3 operator/(Vector3 a, double b);

Vector3 operator*(Quaternion q, Vector3 v);

Quaternion operator*(Quaternion a, Quaternion b);

Quaternion operator*(Quaternion a, double b);

