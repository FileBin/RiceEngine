#pragma once
struct Vector2 {
	Vector2() {}
	Vector2(double x, double y) : x(x), y(y) {}
	double x = 0, y = 0;
	static double Dot(Vector2 a, Vector2 b);

	static const Vector2 zero();
};

struct Vector3 : public Vector2 {
	Vector3() : Vector2() {}
	Vector3(double x, double y) : Vector2(x, y) {}
	Vector3(double x, double y, double z) : Vector2(x, y), z(z) {}
	double x = 0, y = 0, z = 0;
	static double Dot(Vector3 a, Vector3 b);

	static const Vector3 zero();
};

struct Quaternion {
	double x, y, z, w;
};

Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector3 operator+(Vector3 a, Vector3 b);
Vector3 operator-(Vector3 a, Vector3 b);

