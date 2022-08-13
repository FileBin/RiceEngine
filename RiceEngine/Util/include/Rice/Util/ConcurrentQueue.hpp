#pragma once
#include "stdafx.hpp"

#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

NSP_UTIL_BEGIN

template <typename T> class ConcurrentQueue {
  private:
    std::vector<T> vec;
    std::shared_mutex mutex;

    public:
    void push(T item) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        vec.push_back(std::move(item));
    }

    bool try_pop(T& item) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        if (vec.empty()) {
            return false;
        }
        item = std::move(vec.front());
        vec.erase(vec.begin());
        return true;
    }

    bool empty() {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return vec.empty();
    }

    size_t count() {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return vec.size();
    }

    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        vec.clear();
    }

    bool try_peek(T& item) {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (vec.empty()) {
            return false;
        }
        item = vec.front();
        return true;
    }

    T &operator[](size_t idx) {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return vec[idx];
    }
};

NSP_UTIL_END