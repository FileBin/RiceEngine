#include "Rice/defines.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#pragma once

#define DEFAULT_SEARIALIZATION(T)                                              \
    template <> struct serializer<T> {                                         \
        data_t to_bytes(const T &t) {                                          \
            data_t data{sizeof(T)};                                            \
            memcpy((void *)data.data(), (void *)&t, sizeof(T));                \
            return data;                                                       \
        }                                                                      \
        size_t from_bytes(data_t data, T &component) {                         \
            component = *((T *)data.data());                                   \
            return sizeof(T);                                                  \
        }                                                                      \
    };

template <typename T> struct serializer;
template <typename T> struct json_serializer;

template <typename T> struct component_serializer;

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

template <typename T> struct normalize_type { typedef T type; };
template <typename T> struct normalize_type<T &> { typedef T type; };
template <typename T> struct normalize_type<const T &> { typedef T type; };