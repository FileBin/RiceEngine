#include <GameEngine/Vectors.h>
#include <GameEngine\Math.h>
#include <GameEngine\Matrixes.h>
#include <GameEngine\stdafx.h>
#include <GameEngine/Vectors/Quaternion.h>
#include <GameEngine\Vectors\Vector2i.h>

using namespace Game;

#pragma region Constants
 
const Vector3 Vector3::zero = { 0,0,0 };
const Vector3 Vector3::one = { 1,1,1 };
const Vector3 Vector3::left = { -1,0,0 };
const Vector3 Vector3::right = { 1,0,0 };
const Vector3 Vector3::up = { 0,1,0 };
const Vector3 Vector3::down = { 0,-1,0 };
const Vector3 Vector3::forward = { 0,0,1 };
const Vector3 Vector3::backward = { 0,0,-1 };

const Vector2 Vector2::one = { 1,1 };
const Vector2 Vector2::zero = { 0,0 };
const Vector2 Vector2::left = { -1,0 };
const Vector2 Vector2::right = { 1,0 };
const Vector2 Vector2::up = { 0,1 };
const Vector2 Vector2::down = { 0,-1 };

const Quaternion Quaternion::identity = { 0,0,0,1 };

#pragma endregion

#pragma region Vector2
dbl Vector2::Dot(Vector2 A, Vector2 b) {
    return A.x * b.x + A.y * b.y;
}

dbl Vector2::Length() { return sqrt(SqrLength()); }

Vector2 operator+(Vector2 A, Vector2 b) {
    return { A.x + b.x, A.y + b.y };
}

Vector2 operator-(Vector2 A, Vector2 b) {
    return { A.x - b.x, A.y - b.y };
}

Vector2 operator*(Vector2 A, dbl b) {
    return { A.x * b, A.y * b };
}

Vector2 operator/(Vector2 A, dbl b) {
    return { A.x / b, A.y / b };
}

Vector2 Vector2::operator+=(Vector2 v) {
    *this = (*this) + v;
    return *this;
}

Vector2 Vector2::operator-=(Vector2 v) {
    *this = (*this) - v;
    return *this;
}

Vector2 Vector2::operator*=(dbl x) {
    *this = (*this) * x;
    return *this;
}

Vector2 Vector2::operator/=(dbl x) {
    *this = (*this) / x;
    return *this;
}
#pragma endregion

#pragma region Vector2i
bool operator==(Vector2i a, Vector2i b) {
    return (a.x == b.x) && (a.y == b.y);
}
#pragma endregion

#pragma region Vector3
Vector3::Vector3(const Vector3i& v) {
    x = (double)v.x;
    y = (double)v.y;
    z = (double)v.z;
}

double& Vector3::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    throw std::exception("Vector3i is out of range!");
}

double Vector3::Length() {
    return sqrt(SqrLength());
}

double Vector3::SqrLength() {
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalized() {
    auto l = Math::Qrsqrt(SqrLength());
    return { x * l, y * l, z * l };
}

double Vector3::Dot(Vector3 A, Vector3 b) {
    return A.x * b.x + A.y * b.y + A.z * b.z;
}

Vector3 operator-(Vector3 A, Vector3 b) {
    return { A.x - b.x, A.y - b.y, A.z - b.z };
}

Vector3 operator*(Vector3 A, double b) {
    return { A.x * b, A.y * b, A.z * b };
}

Vector3 operator/(Vector3 A, double b) {
    return { A.x / b, A.y / b, A.z / b };
}

Vector3 operator*(Quaternion q, Vector3 v) {
    return v * Matrix4x4::Rotation(q);
}

Vector3 Vector3::Cross(Vector3 left, Vector3 right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x };
}

Vector3 Vector3::Lerp(Vector3 a, Vector3 b, dbl t) {
    return a * (1 - t) + b * t;
}

Vector3& Vector3::operator=(Vector3f& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector3 Vector3::operator*=(Quaternion q) {
    *this = q * (*this);
    return *this;
}

Vector3 Vector3::operator-() {
    return { -x,-y,-z };
}

Vector3 Vector3::operator+=(Vector3 v) {
    *this = *this + v;
    return *this;
}

Vector3 Vector3::operator-=(Vector3 v) {
    *this = *this - v;
    return *this;
}

Vector3 Vector3::operator*=(double v) {
    *this = *this * v;
    return *this;
}

Vector3 Vector3::operator/=(double v) {
    *this = *this / v;
    return *this;
}

Vector3 operator+(Vector3 a, Vector3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}
#pragma endregion

#pragma region Vector3i

bool operator==(Vector3i a, Vector3i b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

Vector3i Vector3i::operator-() {
    return { -x, -y, -z };
}

Vector3i operator+(Vector3i a, Vector3i b) {
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vector3i operator-(Vector3i a, Vector3i b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vector3i operator*(Vector3i a, num b) {
    return {
        a.x * b,
        a.y * b,
        a.z * b
    };
}

Vector3i operator/(Vector3i a, num b) {
    return {
        a.x / b,
        a.y / b,
        a.z / b
    };
}

num Vector3i::SqrLength() {
    return x * x + y * y + z * z;
}

num Vector3i::Dot(Vector3i a, Vector3i b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3i Vector3i::Cross(Vector3i left, Vector3i right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x };
}

Vector3i::Vector3i(const Vector3& v) {
    x = floor(v.x);
    y = floor(v.y);
    z = floor(v.z);
}

num& Vector3i::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    throw std::exception("Vector3i is out of range!");
}

#pragma endregion

#pragma region Vector3f
Vector3f& Vector3f::operator=(Vector3& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector3f& Vector3f::operator+=(Vector3f& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

float Vector3f::Length() {
    return sqrt(SqrLength());
}

float Vector3f::SqrLength() {
    return x*x + y*y + z*z;
}

Vector3f Vector3f::Normalized() {
    auto l = Math::Qrsqrt(SqrLength());
    return { x * l, y * l, z * l };
}

float Vector3f::Dot(Vector3f a, Vector3f b) {
    return a.x * b.x + a.y * b.y * a.z * b.z;
}

Vector3f Vector3f::Cross(Vector3f left, Vector3f right) {
    return {
    left.y * right.z - left.z * right.y,
    left.z * right.x - left.x * right.z,
    left.x * right.y - left.y * right.x };
}

float& Vector3f::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    throw std::exception("Vector3f is out of range!");
}

Vector3f Vector3f::ApplyFunc(std::function<float(float)> func) {
    return {
        func(x),
        func(y),
        func(z)
    };
}

Vector3f Vector3f::ApplyFunc(std::function<float(size_t, float)> func) {
    return {
        func(0, x),
        func(1, y),
        func(2, z)
    };
}

Vector3f operator+(Vector3f a, Vector3f b) {
    return { a.x + b.z, a.y + b.y, a.z + b.z };
}

Vector3f operator-(Vector3f a, Vector3f b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3f operator*(Vector3f a, float b) {
    return { a.x * b, a.y * b, a.z * b };
}

Vector3f operator/(Vector3f a, float b) {
    return { a.x / b, a.y / b, a.z / b };
}
#pragma endregion


#pragma region Quaternion
Quaternion operator*(Quaternion A, double b) {
    return { A.x * b, A.y * b, A.z * b, A.w * b };
}

Quaternion operator*(Quaternion q1, Quaternion q2) {
    return {
        q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
        -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
        q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
        -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w,
    };
}

Quaternion Quaternion::FromAxisAngle(Vector3 axis, double angle) {
    auto halfAngle = angle * .5;
    if (Math::Abs(halfAngle) < .001)
        return identity;
    auto s = Math::Sin(halfAngle);
    Quaternion q;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    q.w = Math::Cos(halfAngle);
    return q;
}

Quaternion Quaternion::LookAt(Vector3 sourcePoint, Vector3 destPoint) {
    Vector3 forwardVector = (destPoint - sourcePoint).Normalized();

    auto dot = Vector3::Dot(Vector3::forward, forwardVector);

    if (Math::Abs(dot + 1.0) < 0.000001) {
        return { 0, 1, 0, 3.1415926535897932 };
    }
    if (Math::Abs(dot - 1.0) < 0.000001) {
        return { 0,0,0,0 }/*Quaternion::identity*/;
    }

    auto rotAngle = Math::Acos(dot);
    Vector3 rotAxis = Vector3::Cross(Vector3::forward, forwardVector);
    rotAxis = (rotAxis).Normalized();
    return Quaternion::FromAxisAngle(rotAxis, rotAngle);
}

Quaternion Quaternion::FromEulerAngles(double x, double y, double z) {
    double cy = Math::Cos(y * 0.5);
    double sy = Math::Sin(z * 0.5);
    double cp = Math::Cos(y * 0.5);
    double sp = Math::Sin(y * 0.5);
    double cr = Math::Cos(x * 0.5);
    double sr = Math::Sin(x * 0.5);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

double Quaternion::Magnitude() {
    auto l = w * w + x * x + y * y + z * z;
    return sqrt(l);
}

Quaternion Quaternion::operator-() {
    return { -x,-y,-z,-w };
}

Quaternion Quaternion::Inversed() {
    auto norm = Magnitude();
    if (norm > 0) {
        norm = 1 / norm;
        return { -x * norm, -y * norm, -z * norm, -w * norm };
    }
    return (*this);
}

Quaternion Quaternion::Opposite() {
    return FromAxisAngle(-ToAxisAngle());
}

Quaternion Quaternion::Normalized() {
    auto l = w * w + x * x + y * y + z * z;
    l = sqrt(l);
    return { x / l, y / l, z / l, w / l };
}

Vector3 Quaternion::ToEulerAngles() {
    Vector3 angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angles.x = Math::Atan2(sinr_cosp, cosr_cosp);

    // yaw (y-axis rotation)
    double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        angles.y = std::copysign(Math::PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.y = Math::Asin(sinp);

    // pitch (z-axis rotation)
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angles.z = Math::Atan2(siny_cosp, cosy_cosp);

    return angles;
}

Vector3 Quaternion::ToAxisAngle() {
    auto angle = 2 * Math::Acos(w);
    auto s = sqrt(1 - w * w);
    if (s > 0) {
        s = 1 / s;
        Vector3 v = { x,y,z };
        v = v * s;
        return v * angle;
    } else {
        return Vector3::zero;
    }
}
#pragma endregion
