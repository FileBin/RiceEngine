#include <GameEngine/Vectors.h>

Vector3 _zero = { 0,0,0 };

double Vector2::Dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

double Vector3::Dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector2 operator+(Vector2 a, Vector2 b) {
    return { a.x + b.x, a.y + b.y };
}

Vector2 operator-(Vector2 a, Vector2 b) {
    return { a.x - b.x, a.y - b.y };
}

Vector3 operator+(Vector3 a, Vector3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 operator-(Vector3 a, Vector3 b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

const Vector2 Vector2::zero() {
    return _zero;
}

const Vector3 Vector3::zero() {
    return _zero;
}
