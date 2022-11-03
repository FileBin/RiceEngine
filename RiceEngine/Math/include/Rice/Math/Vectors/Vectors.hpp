#pragma once

#include "Rice/Util/defines.hpp"

#include "VectorT.hpp"

struct Vector2 : public VectorT<dbl, 2> {
    Vector2() {}
    Vector2(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector2(const VectorT<T, 2> &other) : VectorT(other) {}

    Vector2(dbl x, dbl y) : VectorT({x, y}) {}
    static const Vector2 zero;
    static const Vector2 one;
    static const Vector2 left;
    static const Vector2 right;
    static const Vector2 up;
    static const Vector2 down;
};

struct Vector2i : public VectorT<num, 2> {
    Vector2i() {}
    Vector2i(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector2i(const VectorT<T, 2> &other) : VectorT(other) {}

    Vector2i(num x, num y) : VectorT({x, y}) {}
    static const Vector2i zero;
    static const Vector2i one;
    static const Vector2i left;
    static const Vector2i right;
    static const Vector2i up;
    static const Vector2i down;
};

struct Vector2f : public VectorT<float, 2> {
    Vector2f() {}
    Vector2f(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector2f(const VectorT<T, 2> &other) : VectorT(other) {}

    Vector2f(float x, float y) : VectorT({x, y}) {}
    static const Vector2f zero;
    static const Vector2f one;
    static const Vector2f left;
    static const Vector2f right;
    static const Vector2f up;
    static const Vector2f down;
};

struct Vector3 : public VectorT<dbl, 3> {
    Vector3() {}
    Vector3(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector3(const VectorT<T, 3> &other) : VectorT(other) {}
    Vector3(dbl x, dbl y, dbl z) : VectorT({x, y, z}) {}
    static const Vector3 zero;
    static const Vector3 one;
    static const Vector3 left;
    static const Vector3 right;
    static const Vector3 up;
    static const Vector3 down;
    static const Vector3 forward;
    static const Vector3 backward;
};

struct Vector3i : public VectorT<num, 3> {
    Vector3i() {}
    Vector3i(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector3i(const VectorT<T, 3> &other) : VectorT(other) {}
    Vector3i(num x, num y, num z) : VectorT({x, y, z}) {}
    static const Vector3i zero;
    static const Vector3i one;
    static const Vector3i left;
    static const Vector3i right;
    static const Vector3i up;
    static const Vector3i down;
    static const Vector3i forward;
    static const Vector3i backward;
};

struct Vector3f : public VectorT<float, 3> {
    Vector3f() {}
    Vector3f(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector3f(const VectorT<T, 3> &other) : VectorT(other) {}
    Vector3f(float x, float y, float z) : VectorT({x, y, z}) {}
    static const Vector3f zero;
    static const Vector3f one;
    static const Vector3f left;
    static const Vector3f right;
    static const Vector3f up;
    static const Vector3f down;
    static const Vector3f forward;
    static const Vector3f backward;
};

struct Vector4 : public VectorT<dbl, 4> {
    Vector4() {}
    Vector4(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector4(const VectorT<T, 4> &other) : VectorT(other) {}

    Vector4(dbl x, dbl y, dbl z, dbl w) : VectorT({x, y, z, w}) {}
    static const Vector4 zero;
    static const Vector4 one;
    static const Vector4 left;
    static const Vector4 right;
    static const Vector4 up;
    static const Vector4 down;
    static const Vector4 forward;
    static const Vector4 backward;
    static const Vector4 deeper;
    static const Vector4 higher;
};

struct Vector4i : public VectorT<num, 4> {
    Vector4i() {}
    Vector4i(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector4i(const VectorT<T, 4> &other) : VectorT(other) {}

    Vector4i(num x, num y, num z, num w) : VectorT({x, y, z, w}) {}
    static const Vector4i zero;
    static const Vector4i one;
    static const Vector4i left;
    static const Vector4i right;
    static const Vector4i up;
    static const Vector4i down;
    static const Vector4i forward;
    static const Vector4i backward;
    static const Vector4i deeper;
    static const Vector4i higher;
};

struct Vector4f : public VectorT<float, 4> {
    Vector4f() {}
    Vector4f(const VectorT &other) : VectorT(other) {}
    template <typename T> explicit Vector4f(const VectorT<T, 4> &other) : VectorT(other) {}

    Vector4f(float x, float y, float z, float w) : VectorT({x, y, z, w}) {}
    static const Vector4f zero;
    static const Vector4f one;
    static const Vector4f left;
    static const Vector4f right;
    static const Vector4f up;
    static const Vector4f down;
    static const Vector4f forward;
    static const Vector4f backward;
    static const Vector4f deeper;
    static const Vector4f higher;
};