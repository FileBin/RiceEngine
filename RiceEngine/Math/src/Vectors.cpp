#include "Rice/Math/Math.hpp"
#include "pch.h"

#include "Rice/Util/Util.hpp"

using namespace Rice;

#pragma region Constants

const Vector2 Vector2::one = {1, 1};
const Vector2 Vector2::zero = {0, 0};
const Vector2 Vector2::left = {-1, 0};
const Vector2 Vector2::right = {1, 0};
const Vector2 Vector2::up = {0, 1};
const Vector2 Vector2::down = {0, -1};

const Vector2i Vector2i::one = {1, 1};
const Vector2i Vector2i::zero = {0, 0};
const Vector2i Vector2i::left = {-1, 0};
const Vector2i Vector2i::right = {1, 0};
const Vector2i Vector2i::up = {0, 1};
const Vector2i Vector2i::down = {0, -1};

const Vector2f Vector2f::one = {1, 1};
const Vector2f Vector2f::zero = {0, 0};
const Vector2f Vector2f::left = {-1, 0};
const Vector2f Vector2f::right = {1, 0};
const Vector2f Vector2f::up = {0, 1};
const Vector2f Vector2f::down = {0, -1};

const Vector3 Vector3::zero = {0, 0, 0};
const Vector3 Vector3::one = {1, 1, 1};
const Vector3 Vector3::left = {-1, 0, 0};
const Vector3 Vector3::right = {1, 0, 0};
const Vector3 Vector3::up = {0, 1, 0};
const Vector3 Vector3::down = {0, -1, 0};
const Vector3 Vector3::forward = {0, 0, 1};
const Vector3 Vector3::backward = {0, 0, -1};

const Vector3i Vector3i::zero = {0, 0, 0};
const Vector3i Vector3i::one = {1, 1, 1};
const Vector3i Vector3i::left = {-1, 0, 0};
const Vector3i Vector3i::right = {1, 0, 0};
const Vector3i Vector3i::up = {0, 1, 0};
const Vector3i Vector3i::down = {0, -1, 0};
const Vector3i Vector3i::forward = {0, 0, 1};
const Vector3i Vector3i::backward = {0, 0, -1};

const Vector3f Vector3f::zero = {0, 0, 0};
const Vector3f Vector3f::one = {1, 1, 1};
const Vector3f Vector3f::left = {-1, 0, 0};
const Vector3f Vector3f::right = {1, 0, 0};
const Vector3f Vector3f::up = {0, 1, 0};
const Vector3f Vector3f::down = {0, -1, 0};
const Vector3f Vector3f::forward = {0, 0, 1};
const Vector3f Vector3f::backward = {0, 0, -1};

const Vector4 Vector4::zero = {0, 0, 0, 0};
const Vector4 Vector4::one = {1, 1, 1, 1};
const Vector4 Vector4::left = {-1, 0, 0, 0};
const Vector4 Vector4::right = {1, 0, 0, 0};
const Vector4 Vector4::up = {0, 1, 0, 0};
const Vector4 Vector4::down = {0, -1, 0, 0};
const Vector4 Vector4::forward = {0, 0, 1, 0};
const Vector4 Vector4::backward = {0, 0, -1, 0};
const Vector4 Vector4::higher = {0, 0, 0, 1};
const Vector4 Vector4::deeper = {0, 0, 0, -1};

const Vector4i Vector4i::zero = {0, 0, 0, 0};
const Vector4i Vector4i::one = {1, 1, 1, 1};
const Vector4i Vector4i::left = {-1, 0, 0, 0};
const Vector4i Vector4i::right = {1, 0, 0, 0};
const Vector4i Vector4i::up = {0, 1, 0, 0};
const Vector4i Vector4i::down = {0, -1, 0, 0};
const Vector4i Vector4i::forward = {0, 0, 1, 0};
const Vector4i Vector4i::backward = {0, 0, -1, 0};
const Vector4i Vector4i::higher = {0, 0, 0, 1};
const Vector4i Vector4i::deeper = {0, 0, 0, -1};

const Vector4f Vector4f::zero = {0, 0, 0, 0};
const Vector4f Vector4f::one = {1, 1, 1, 1};
const Vector4f Vector4f::left = {-1, 0, 0, 0};
const Vector4f Vector4f::right = {1, 0, 0, 0};
const Vector4f Vector4f::up = {0, 1, 0, 0};
const Vector4f Vector4f::down = {0, -1, 0, 0};
const Vector4f Vector4f::forward = {0, 0, 1, 0};
const Vector4f Vector4f::backward = {0, 0, -1, 0};
const Vector4f Vector4f::higher = {0, 0, 0, 1};
const Vector4f Vector4f::deeper = {0, 0, 0, -1};

const Quaternion Quaternion::identity = {0, 0, 0, 1};

#pragma endregion

Vector3 operator*(Quaternion q, Vector3 v) { return v * Matrix4x4::rotation(q); }

const Vector3 &operator*=(Vector3 &v, Quaternion q) { return v = v * Matrix4x4::rotation(q); }

/*#pragma region Vector2
Vector2::Vector2(const Vector2i& v){
        x = v.x;
        y = v.y;
}

dbl Vector2::Dot(Vector2 A, Vector2 b) {
    return A.x * b.x + A.y * b.y;
}

dbl Vector2::SqrLength() { return x * x + y * y; };

dbl Vector2::Length() { return sqrt(SqrLength()); }

Vector2 Vector2::Normalized() {
    return *this / Length();
}

double& Vector2::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

Vector2 Vector2::operator-() {
    return { -x,-y };
}

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

Vector2 operator*(Vector2 a, Vector2 b) {
    return { a.x * b.x, a.y * b.y };
}

Vector2 operator/(Vector2 a, Vector2 b) {
    return { a.x / b.x, a.y / b.y };
}

Vector2& Vector2::operator+=(Vector2 v) {
    *this = (*this) + v;
    return *this;
}

Vector2& Vector2::operator-=(Vector2 v) {
    *this = (*this) - v;
    return *this;
}

Vector2& Vector2::operator*=(dbl x) {
    *this = (*this) * x;
    return *this;
}

Vector2& Vector2::operator/=(dbl x) {
    *this = (*this) / x;
    return *this;
}

Vector2& Vector2::operator/=(Vector2 x) {
    *this = (*this) / x;
    return *this;
}

bool operator==(Vector2 a, Vector2 b) {
    return (a.x == b.x) && (a.y == b.y);
}
#pragma endregion

#pragma region Vector2i
dbl Vector2i::Dot(Vector2i a, Vector2i b) {
    return a.x * b.x + a.y * b.y;
}

dbl Vector2i::SqrLength() { return x * x + y * y; };

dbl Vector2i::Length() { return sqrt(SqrLength()); }

num& Vector2i::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

Vector2i Vector2i::operator-() {
    return { -x,-y };
}

Vector2i operator+(Vector2i a, Vector2i b) {
    return { a.x + b.x, a.y + b.y };
}

Vector2i operator-(Vector2i a, Vector2i b) {
    return { a.x - b.x, a.y - b.y };
}

Vector2i operator*(Vector2i a, num b) {
    return { a.x * b, a.y * b };
}

Vector2i operator/(Vector2i a, num b) {
    return { a.x / b, a.y / b };
}

Vector2i& Vector2i::operator+=(Vector2i v) {
    *this = (*this) + v;
    return *this;
}

Vector2i& Vector2i::operator-=(Vector2i v) {
    *this = (*this) - v;
    return *this;
}

Vector2i& Vector2i::operator*=(num x) {
    *this = (*this) * x;
    return *this;
}

Vector2i& Vector2i::operator/=(num x) {
    *this = (*this) / x;
    return *this;
}

bool operator==(Vector2i a, Vector2i b) {
    return (a.x == b.x) && (a.y == b.y);
}
#pragma endregion

#pragma region Vector2f
dbl Vector2f::Dot(Vector2f a, Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

dbl Vector2f::SqrLength() { return x * x + y * y; };

dbl Vector2f::Length() { return sqrt(SqrLength()); }

float& Vector2f::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

Vector2f Vector2f::operator-() {
    return { -x, -y };
}

Vector2f operator+(Vector2f a, Vector2f b) {
    return { a.x + b.x, a.y + b.y };
}

Vector2f operator-(Vector2f a, Vector2f b) {
    return { a.x - b.x, a.y - b.y };
}

Vector2f operator*(Vector2f a, float b) {
    return { a.x * b, a.y * b };
}

Vector2f operator/(Vector2f a, float b) {
    return { a.x / b, a.y / b };
}

Vector2f& Vector2f::operator+=(Vector2f v) {
    *this = (*this) + v;
    return *this;
}

Vector2f& Vector2f::operator-=(Vector2f v) {
    *this = (*this) - v;
    return *this;
}

Vector2f& Vector2f::operator*=(float x) {
    *this = (*this) * x;
    return *this;
}

Vector2f& Vector2f::operator/=(float x) {
    *this = (*this) / x;
    return *this;
}

bool operator==(Vector2f a, Vector2f b) {
    return (a.x == b.x) && (a.y == b.y);
}
#pragma endregion

#pragma region Vector3
Vector3::Vector3(const Vector3i& v) {
    x = (dbl)v.x;
    y = (dbl)v.y;
    z = (dbl)v.z;
}

Vector3::Vector3(const Vector3f& v) {
    x = (dbl)v.x;
    y = (dbl)v.y;
    z = (dbl)v.z;
}

Vector3::Vector3(const Vector2& v) {
    x = (dbl)v.x;
    y = (dbl)v.y;
    z = 0;
}

double& Vector3::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

double Vector3::Length() {
    return sqrt(SqrLength());
}

double Vector3::SqrLength() {
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalized() {
    auto l = 1 / sqrt(SqrLength());
    return { x * l, y * l, z * l };
}

void Vector3::Normalize() {
    auto l = 1 / sqrt(SqrLength());
    x *= l;
    y *= l;
    z *= l;
}

void Vector3::Qnormalize() {
    auto l = Math::qrsqrt(SqrLength());
    x *= l;
    y *= l;
    z *= l;
}

double Vector3::Dot(Vector3 A, Vector3 b) {
    return A.x * b.x + A.y * b.y + A.z * b.z;
}

Vector3 operator-(Vector3 A, Vector3 b) {
    return { A.x - b.x, A.y - b.y, A.z - b.z };
}

Vector3 operator*(Quaternion q, Vector3 v) {
    return v * Matrix4x4::rotation(q);
}

Vector3 Vector3::cross(Vector3 left, Vector3 right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x };
}

Vector3 Vector3::Lerp(Vector3 a, Vector3 b, dbl t) {
    return a * (1 - t) + b * t;
}

Vector3 Vector3::ProjectOnPlane(Vector3 vec, Vector3 planeNormal) {
    return vec - planeNormal * Dot(vec, planeNormal);
}

Vector3& Vector3::operator=(Vector3f& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector3& Vector3::operator*=(dbl q) {
    *this = (*this) * q;
    return *this;
}

Vector3& Vector3::operator/=(dbl q) {
    *this = (*this) / q;
    return *this;
}

Vector3 Vector3::operator*=(Quaternion q) {
    *this = q * (*this);
    return *this;
}

Vector3 Vector3::operator-() const {
    return { -x,-y,-z };
}

Vector3& Vector3::operator+=(Vector3 v) {
    *this = *this + v;
    return *this;
}

Vector3& Vector3::operator-=(Vector3 v) {
    *this = *this - v;
    return *this;
}

Vector3 operator+(Vector3 a, Vector3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 operator*(Vector3 a, dbl b) {
    return { a.x * b , a.y * b, a.z * b };
}

Vector3 operator/(Vector3 a, dbl b) {
    return { a.x / b , a.y / b, a.z / b };
}

Vector3 operator/(dbl b, Vector3 a) {
    return { b / a.x , b / a.y, b / a.z };
}

bool operator==(Vector3 a, Vector3 b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}
#pragma endregion

#pragma region Vector3i
Vector3i::Vector3i(const Vector3& v) {
    x = (int)v.x;
    y = (int)v.y;
    z = (int)v.z;
}

num& Vector3i::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

double Vector3i::Length() {
    return sqrt(SqrLength());
}

double Vector3i::SqrLength() {
    return x * x + y * y + z * z;
}

num Vector3i::Dot(Vector3i a, Vector3i b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3i operator-(Vector3i A, Vector3i b) {
    return { A.x - b.x, A.y - b.y, A.z - b.z };
}

Vector3i operator*(Quaternion q, Vector3i v) {
    return v * Matrix4x4::rotation(q);
}

Vector3i Vector3i::Cross(Vector3i left, Vector3i right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x };
}

Vector3i& Vector3i::operator=(Vector3& v) {
    x = (int)v.x;
    y = (int)v.y;
    z = (int)v.z;
    return *this;
}

Vector3i& Vector3i::operator*=(num q) {
    *this = (*this) * q;
    return *this;
}

Vector3i& Vector3i::operator/=(num q) {
    *this = (*this) / q;
    return *this;
}

Vector3i Vector3i::operator-() {
    return { -x,-y,-z };
}

Vector3i& Vector3i::operator+=(Vector3i v) {
    *this = *this + v;
    return *this;
}

Vector3i& Vector3i::operator-=(Vector3i v) {
    *this = *this - v;
    return *this;
}

Vector3i operator+(Vector3i a, Vector3i b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3i operator*(Vector3i a, num b) {
    return { a.x * b , a.y * b, a.z * b };
}

Vector3i operator/(Vector3i a, num b) {
    return { a.x / b , a.y / b, a.z / b };
}

bool operator==(Vector3i a, Vector3i b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}
#pragma endregion

#pragma region Vector3f
Vector3f::Vector3f(const Vector3& v) {
    x = (float)v.x;
    y = (float)v.y;
    z = (float)v.z;
}

float& Vector3f::operator[](size_t idx) {
    if (idx == 0)
        return x;
    if (idx == 1)
        return y;
    if (idx == 2)
        return z;
    THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
}

double Vector3f::length() {
    return sqrt(sqrLength());
}

double Vector3f::sqrLength() {
    return x * x + y * y + z * z;
}

Vector3f Vector3f::normalized() {
    float l = 1 / sqrt(sqrLength());
    return { x * l, y * l, z * l };
}

void Vector3f::normalize() {
    float l = 1 / sqrt(sqrLength());
    x *= l;
    y *= l;
    z *= l;
}

void Vector3f::qnormalize() {
    float l = Math::qrsqrt(sqrLength());
    x *= l;
    y *= l;
    z *= l;
}

double Vector3f::dot(Vector3f a, Vector3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3f operator-(Vector3f a, Vector3f b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3f operator*(Quaternion q, Vector3f v) {
    return (Vector3)v * Matrix4x4::rotation(q);
}

Vector3f Vector3f::cross(Vector3f left, Vector3f right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x };
}

Vector3f Vector3f::lerp(Vector3f a, Vector3f b, float t) {
    return a * (1 - t) + b * t;
}

Vector3f& Vector3f::operator=(Vector3& v) {
    x = (float)v.x;
    y = (float)v.y;
    z = (float)v.z;
    return *this;
}

Vector3f& Vector3f::operator*=(float q) {
    *this = (*this) * q;
    return *this;
}

Vector3f& Vector3f::operator/=(float q) {
    *this = (*this) / q;
    return *this;
}

Vector3f Vector3f::operator-() {
    return { -x,-y,-z };
}

Vector3f& Vector3f::operator+=(Vector3f v) {
    *this = *this + v;
    return *this;
}

Vector3f& Vector3f::operator-=(Vector3f v) {
    *this = *this - v;
    return *this;
}

Vector3f operator+(Vector3f a, Vector3f b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3f operator*(Vector3f a, float b) {
    return { a.x * b , a.y * b, a.z * b };
}

Vector3f operator/(Vector3f a, float b) {
    return { a.x / b , a.y / b, a.z / b };
}

bool operator==(Vector3f a, Vector3f b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}
#pragma endregion

#pragma region Vector4f
Vector4f::Vector4f(float _x, float _y, float _z, float _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

#pragma endregion*/

#pragma region Quaternion
Quaternion operator*(Quaternion A, double b) { return {A.x * b, A.y * b, A.z * b, A.w * b}; }

Quaternion operator*(Quaternion q1, Quaternion q2) {
    return {
        q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
        -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
        q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
        -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w,
    };
}

Quaternion Quaternion::fromAxisAngle(Vector3 axis, double angle) {
    auto halfAngle = angle * .5;
    if (Math::abs(halfAngle) < .001)
        return identity;
    auto s = Math::sin(halfAngle);
    Quaternion q;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    q.w = Math::cos(halfAngle);
    return q;
}

Quaternion Quaternion::lookAt(Vector3 sourcePoint, Vector3 destPoint) {
    Vector3 forwardVector = (destPoint - sourcePoint).normalized();

    auto dot = Vector3::dot(Vector3::forward, forwardVector);

    if (Math::abs(dot + 1.0) < 0.000001) {
        return {0, 1, 0, 3.1415926535897932};
    }
    if (Math::abs(dot - 1.0) < 0.000001) {
        return {0, 0, 0, 0} /*Quaternion::identity*/;
    }

    auto rotAngle = Math::acos(dot);
    Vector3 rotAxis = Vector3::cross(Vector3::forward, forwardVector);
    rotAxis = (rotAxis).normalized();
    return Quaternion::fromAxisAngle(rotAxis, rotAngle);
}

Quaternion Quaternion::fromEulerAngles(double x, double y, double z) {
    double cy = Math::cos(z * 0.5);
    double sy = Math::sin(z * 0.5);
    double cp = Math::cos(y * 0.5);
    double sp = Math::sin(y * 0.5);
    double cr = Math::cos(x * 0.5);
    double sr = Math::sin(x * 0.5);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

double Quaternion::magnitude() {
    auto l = w * w + x * x + y * y + z * z;
    return sqrt(l);
}

Quaternion Quaternion::operator-() { return {-x, -y, -z, -w}; }

Quaternion Quaternion::inversed() {
    auto norm = magnitude();
    if (norm > 0) {
        norm = 1 / norm;
        return {-x * norm, -y * norm, -z * norm, -w * norm};
    }
    return (*this);
}

Quaternion Quaternion::opposite() { return fromAxisAngle(-toAxisAngle()); }

Quaternion Quaternion::normalized() {
    auto l = w * w + x * x + y * y + z * z;
    l = sqrt(l);
    return {x / l, y / l, z / l, w / l};
}

Vector3 Quaternion::toEulerAngles() {
    Vector3 angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angles.x = Math::atan(sinr_cosp, cosr_cosp);

    // yaw (y-axis rotation)
    double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        angles.y = std::copysign(Math::PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.y = Math::asin(sinp);

    // pitch (z-axis rotation)
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angles.z = Math::atan(siny_cosp, cosy_cosp);

    return angles;
}

Vector3 Quaternion::toAxisAngle() {
    auto angle = 2 * Math::acos(w);
    auto s = sqrt(1 - w * w);
    if (s > 0) {
        s = 1 / s;
        Vector3 v = {x, y, z};
        v = v * s;
        return v * angle;
    } else {
        return Vector3::zero;
    }
}
#pragma endregion
