#include "stdafx.hpp"

#include <cstddef>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

NSP_UTIL_BEGIN

template <typename T> class ConcurrentArray {
  private:
    std::vector<T> vec;
    std::vector<uptr<std::shared_mutex>> mutexes;
    size_t m_size, m_count;
    std::shared_mutex &getMutex(size_t index) {
        index %= m_count;
        return *mutexes[index];
    }

  public:
    ConcurrentArray(size_t n, size_t m = 1) {
        m_size = n;
        m_count = m;
        vec.resize(n);
        for (size_t i = 0; i < m; i++) {
            mutexes.push_back(std::make_unique<std::shared_mutex>());
        }
    }

    size_t size() { return m_size; }

    const T& getElement(size_t i) {
        std::shared_lock l(getMutex(i));
        return vec[i];
    }

    void setElement(size_t i, T t) {
        std::unique_lock l(getMutex(i));
        vec[i] = t;
    }
};

NSP_UTIL_END