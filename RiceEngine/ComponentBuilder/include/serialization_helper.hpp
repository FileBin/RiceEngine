#include "Rice/defines.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#pragma once

#define DEFAULT_SEARIALIZATION(T)                                              \
    template <> struct serializator<T> {                                       \
        data_t to_bytes(const T &t) {                                          \
            data_t data{sizeof(T)};                                            \
            memcpy((void *)data.data(), (void *)&t, sizeof(T));                \
            return data;                                                       \
        }                                                                      \
        size_t from_bytes(data_t data, T &component) {                         \
            component = *((T *)data.data());                                   \
            return sizeof(T);                                                  \
        }                                                                      \
    };                                                                         \
                                                                               \
    template <> struct serializator<T &> {                                     \
        data_t to_bytes(const T &t) {                                          \
            data_t data{sizeof(T)};                                            \
            memcpy((void *)data.data(), (void *)&t, sizeof(T));                \
            return data;                                                       \
        }                                                                      \
        size_t from_bytes(data_t data, T &component) {                         \
            component = *((T *)data.data());                                   \
            return sizeof(T);                                                  \
        }                                                                      \
    }

template <typename T> struct serializator;

DEFAULT_SEARIALIZATION(int8_t);
DEFAULT_SEARIALIZATION(uint8_t);
DEFAULT_SEARIALIZATION(int16_t);
DEFAULT_SEARIALIZATION(uint16_t);
DEFAULT_SEARIALIZATION(int32_t);
DEFAULT_SEARIALIZATION(uint32_t);
DEFAULT_SEARIALIZATION(int64_t);
DEFAULT_SEARIALIZATION(uint64_t);

DEFAULT_SEARIALIZATION(float);
DEFAULT_SEARIALIZATION(double);
DEFAULT_SEARIALIZATION(long double);

DEFAULT_SEARIALIZATION(char);
DEFAULT_SEARIALIZATION(wchar_t);

#undef DEFAULT_SEARIALIZATION