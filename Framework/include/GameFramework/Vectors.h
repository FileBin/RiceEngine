#pragma once
struct Vector2 {
	Vector2();
	Vector2(double x, double y);
	double x, y;
	static double Dot(Vector2 a, Vector2 b);

	static const Vector2 zero();
};

Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);

