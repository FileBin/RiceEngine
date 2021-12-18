#pragma once

struct Quaternion;
struct Vector3f;
struct Vector3i;

struct Vector2 {
	Vector2() {}
	Vector2(double x, double y) : x(x), y(y) {}
	double x = 0, y = 0;
	static double Dot(Vector2 A, Vector2 b);

	Vector2 operator-=(Vector2 v);
	Vector2 operator+=(Vector2 v);
	Vector2 operator*=(double v);
	Vector2 operator/=(double v);

	double Length();
	double SqrLength() { return x * x + y * y; }

	static const Vector2 zero;
	static const Vector2 one;
	static const Vector2 left;
	static const Vector2 right;
	static const Vector2 up;
	static const Vector2 down;
};

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

struct Vector2f {
	float x, y;
};

struct Vector3f {
	float x, y, z;
	Vector3f& operator=(Vector3& v);
	Vector3f& operator+=(Vector3f& v);
};

struct Vector3i {
public:
	long long x, y, z;
	Vector3i() = default;
	Vector3i(long long x, long long y, long long z) : x(x), y(y), z(z) {}
	Vector3i(Vector3& vec);
	long long& operator[](size_t idx);
};


struct Vector4f {
	float x, y, z, w;
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

Vector2 operator+(Vector2 A, Vector2 b);
Vector2 operator-(Vector2 A, Vector2 b);
Vector2 operator*(Vector2 A, double b);
Vector2 operator/(Vector2 A, double b);
Vector3 operator+(Vector3 A, Vector3 b);
Vector3 operator-(Vector3 A, Vector3 b);
Vector3 operator*(Vector3 A, double b);
Vector3 operator/(Vector3 A, double b);

Vector3 operator*(Quaternion q, Vector3 v);

Quaternion operator*(Quaternion A, Quaternion b);

Quaternion operator*(Quaternion A, double b);

