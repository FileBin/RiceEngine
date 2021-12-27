#pragma once
struct Vector3;

struct Vector3f {
	float x, y, z;
	Vector3f& operator=(Vector3& v);
	Vector3f& operator+=(Vector3f& v);

	float Length();
	float SqrLength();

	Vector3f Normalized();

	static float Dot(Vector3f a, Vector3f b);
	static Vector3f Cross(Vector3f a, Vector3f b);
};

Vector3f operator+(Vector3f a, Vector3f b);
Vector3f operator-(Vector3f a, Vector3f b);
Vector3f operator*(Vector3f a, float b);
Vector3f operator/(Vector3f a, float b);

//Vector3f operator*(Quaternion q, Vector3f v);