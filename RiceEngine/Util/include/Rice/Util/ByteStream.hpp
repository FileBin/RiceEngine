#include "Rice/Util/namespaces.h"
#include "stdafx.hpp"

#pragma once

NSP_UTIL_BEGIN

class ByteStream {
    data_t &data;

  public:
    ByteStream(data_t &data) : data(data) {}

    template <typename T> T readBinary() {
        T value;
        auto d = read(sizeof(T));
        memcpy(&value, d.data(), sizeof(T));
        return value;
    }

    template<typename T> void writeBinary(T value) {
        auto data = data_t(sizeof(T));
        memcpy(data.data(), &value, sizeof(T));
        write(data);
    }

    data_t read(size_t count) {
        auto range_start = data.begin();
        auto range_end = range_start + count;
        data_t range{range_start, range_end};
        data.erase(range_start, range_end);
        return range;
    }

    void write(data_t d) { data.insert(data.end(), d.begin(), d.end()); }

    bool empty() { return data.empty(); }

    data_t &getData() { return data; }
};

NSP_UTIL_END