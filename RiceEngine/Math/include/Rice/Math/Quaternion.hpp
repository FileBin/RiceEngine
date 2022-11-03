struct Quaternion;

#pragma once

#include "Vectors/Vectors.hpp"

struct Quaternion {
    dbl x, y, z, w;

    static Quaternion fromAxisAngle(Vector3 axis) {
        return fromAxisAngle(axis.normalized(), axis.length());
    };
    static Quaternion fromAxisAngle(Vector3 axis, double angle);
    static Quaternion lookAt(Vector3 sourcePoint, Vector3 destPoint);
    static Quaternion fromEulerAngles(Vector3 v) { return fromEulerAngles(v.x, v.y, v.z); }
    static Quaternion fromEulerAngles(double x, double y, double z);

    dbl magnitude();

    Quaternion operator-();
    Quaternion inversed();
    Quaternion opposite();
    Quaternion normalized();

    Vector3 toEulerAngles();
    Vector3 toAxisAngle();

    const static Quaternion identity;
};

Quaternion operator*(Quaternion a, Quaternion b);

Quaternion operator*(Quaternion a, dbl b);

Vector3 operator*(Quaternion a, Vector3 b);

const Vector3 &operator*=(Vector3 &v, Quaternion q);
