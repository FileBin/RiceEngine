#pragma once

struct Vector3i;
struct Quaternion;
struct Vector3f;

struct Vector3 {
	Vector3() = default;
	Vector3(double x, double y) : x(x), y(y) {}
	Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
	Vector3(Vector3i& vec);
	double x = 0, y = 0, z = 0;

	double Length();
	double SqrLength();

	Vector3 Normalized();

	static double Dot(Vector3 A, Vector3 b);
	static Vector3 Cross(Vector3 A, Vector3 b);

	Vector3 operator-();

	Vector3 operator-=(Vector3 v);
	Vector3 operator+=(Vector3 v);
	Vector3 operator*=(double v);
	Vector3 operator*=(Quaternion q);
	Vector3 operator/=(double v);
	Vector3& operator=(Vector3f& v);

	double& operator[](size_t idx);

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 left;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 forward;
	static const Vector3 backward;
};

Vector3 operator+(Vector3 A, Vector3 b);
Vector3 operator-(Vector3 A, Vector3 b);
Vector3 operator*(Vector3 A, double b);
Vector3 operator/(Vector3 A, double b);

Vector3 operator*(Quaternion q, Vector3 v);