/*
 * file: VectorT.hpp
 * author: filebin
 * date: 2022-11-03 13:19:07
 */

#pragma once

#include "Rice/Util/String.hpp"
#include "VectorData.hpp"

#include <Rice/Util/String.hpp>

#include <cmath>
#include <cstddef>
#include <cstring>
#include <string>

template <typename T, size_t N> struct VectorT : public VectorData<T, N> {
  public:
    constexpr static size_t size = N;
    VectorT() { memset(this->dim, 0, N * sizeof(T)); }
    VectorT(T dim[N]) { memcpy(this->dim, dim, N * sizeof(T)); }
    VectorT(VectorData<T, N> data) { memcpy(this->dim, data.dim, N * sizeof(T)); }

    template <typename U> explicit VectorT(const VectorT<U, N> &other) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] = T(other.dim[i]);
        }
    }

    VectorT &operator+=(VectorT v) {
        for (size_t i = 0; i < N; i++) {
            this->dim[i] += v.dim[i];
        }
        return *this;
    }

    VectorT &operator-=(VectorT v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] -= v.dim[i];
        }
        return *this;
    }

    VectorT &operator*=(VectorT v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] *= v.dim[i];
        }
        return *this;
    }
    VectorT &operator/=(VectorT v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] /= v.dim[i];
        }
        return *this;
    }

    VectorT &operator+=(T v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] += v;
        }
        return *this;
    }

    VectorT &operator-=(T v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] -= v;
        }
        return *this;
    }

    VectorT &operator*=(T v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] *= v;
        }
        return *this;
    }
    VectorT &operator/=(T v) {
        for (size_t i = 0; i < N; ++i) {
            this->dim[i] /= v;
        }
        return *this;
    }

    T &operator[](size_t idx) { return this->dim[idx]; }
    VectorT operator-() {
        VectorT vec;
        for (size_t i = 0; i < N; ++i) {
            vec.dim[i] = -this->dim[i];
        }
        return vec;
    }

    T sqrLength() const {
        T l = 0;
        for (size_t i = 0; i < N; ++i) {
            l += this->dim[i] * this->dim[i];
        }
        return l;
    }
    T length() const { return sqrt(sqrLength()); }

    const VectorT &normalize() {
        (*this) /= length();
        return *this;
    }
    const VectorT &qNormalize();

    VectorT normalized() const { return (*this) / length(); }
    VectorT qNormalized() const;

    static T dot(VectorT a, VectorT b) {
        T sum = 0;
        for (size_t i = 0; i < N; ++i) {
            sum += a.dim[i] * b.dim[i];
        }
        return sum;
    }

    static VectorT<T, 3> cross(VectorT<T, 3> left, VectorT<T, 3> right) {
        return VectorT<T, 3>({left.y * right.z - left.z * right.y,
                              left.z * right.x - left.x * right.z,
                              left.x * right.y - left.y * right.x});
    }

    static VectorT lerp(VectorT a, VectorT b, T t) { return a * (1 - t) + b * t; }
    static VectorT projectOnPlane(VectorT vec, VectorT planeNormal) {
        return vec - planeNormal * dot(vec, planeNormal);
    }

    String toString(String div = ", ") const {
        String result = "(" + std::to_string(this->dim[0]);
        for (size_t i = 1; i < N; ++i) {
            result += div + std::to_string(this->dim[i]);
        }
        result += String(")");
        return result;
    }
};

#include "Rice/Math/Math.hpp"

template <typename T, size_t N> VectorT<T, N> VectorT<T, N>::qNormalized() const {
    return (*this) / Rice::MathT<T>::qrsqrt(sqrLength());
}

template <typename T, size_t N> const VectorT<T, N> &VectorT<T, N>::qNormalize() {
    (*this) /= Rice::MathT<T>::qrsqrt(sqrLength());
    return *this;
}

template <typename T, size_t N>
bool operator==(const VectorT<T, N> &a, const VectorT<T, N> &b) {
    for (size_t i = 0; i < N; i++) {
        if (a.dim[i] != b.dim[i])
            return false;
    }
    return true;
}

template <typename T, size_t N>
VectorT<T, N> operator+(const VectorT<T, N> &a, const VectorT<T, N> &v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; i++) {
        res.dim[i] = a.dim[i] + v.dim[i];
    }
    return res;
}

template <typename T, size_t N>
VectorT<T, N> operator-(const VectorT<T, N> &a, const VectorT<T, N> &v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] - v.dim[i];
    }
    return res;
}

template <typename T, size_t N>
VectorT<T, N> operator*(const VectorT<T, N> &a, const VectorT<T, N> &v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] * v.dim[i];
    }
    return res;
}

template <typename T, size_t N>
VectorT<T, N> operator/(const VectorT<T, N> &a, const VectorT<T, N> &v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] / v.dim[i];
    }
    return res;
}

template <typename T, size_t N> VectorT<T, N> operator+(const VectorT<T, N> &a, T v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; i++) {
        res.dim[i] = a.dim[i] + v;
    }
    return res;
}

template <typename T, size_t N> VectorT<T, N> operator-(const VectorT<T, N> &a, T v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] - v;
    }
    return res;
}

template <typename T, size_t N> VectorT<T, N> operator*(const VectorT<T, N> &a, T v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] * v;
    }
    return res;
}

template <typename T, size_t N> VectorT<T, N> operator/(const VectorT<T, N> &a, T v) {
    VectorT<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res.dim[i] = a.dim[i] / v;
    }
    return res;
}
