/*
 * file: VectorData.hpp
 * author: filebin
 * date: 2022-11-03 13:26:41
 */

#pragma once

template <typename T, size_t N> struct VectorData {
    T dim[N];
};

template <typename T> struct VectorData<T, 4> {
    union {
        T dim[4];
        struct {
            T x, y, z, w;
        };
    };
};

template <typename T> struct VectorData<T, 3> {
    union {
        T dim[3];
        struct {
            T x, y, z;
        };
    };
};

template <typename T> struct VectorData<T, 2> {
    union {
        T dim[2];
        struct {
            T x, y;
        };
    };
};

template <typename T> struct VectorData<T, 1> {
    union {
        T dim[1];
        T x;
    };
};