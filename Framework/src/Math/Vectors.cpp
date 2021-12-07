#include "Vectors.h"

Vector2 _zero = Vector2(0,0);

Vector2::Vector2(): x(0), y(0) {}

Vector2::Vector2(double x, double y): x(x), y(y) {}

double Vector2::Dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

Vector2 operator+(Vector2 a, Vector2 b) {
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(Vector2 a, Vector2 b) {
    return Vector2(a.x - b.x, a.y - b.y);
}

const Vector2 Vector2::zero() {
    return _zero;
}
