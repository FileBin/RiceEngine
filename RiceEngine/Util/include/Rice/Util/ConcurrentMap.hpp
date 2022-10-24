#include "defines.hpp"
#include "stdafx.hpp"

#include <cstddef>
#include <functional>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

NSP_UTIL_BEGIN

template <typename K, typename V, size_t N = 0x40, typename H = std::hash<K>,
          typename E = std::equal_to<K>, typename A = std::allocator<pair<const K, V>>>
class ConcurrentMap {
  public:
    typedef std::pair<K, V> elem_t;
    typedef std::shared_lock<std::shared_mutex> read_lock;
    typedef std::unique_lock<std::shared_mutex> write_lock;

  private:
    struct collision_list {
        std::list<elem_t> list;
        std::shared_mutex mutex;
    };
    collision_list collision_lists[N];

  public:
    void update(K key, V item) {
        auto &l = get_list(key);
        read_lock lock(l.mutex);
        for (auto &e : l.list) {
            if (E()(e.first, key)) {
                lock.unlock();
                write_lock wlock(l.mutex);
                e.second = item;
                return;
            }
        }
        lock.unlock();
        write_lock wlock(l.mutex);
        l.list.push_back(std::make_pair(key, item));
    }

    bool has(K key) {
        auto &l = get_list(key);
        read_lock lock(l.mutex);
        for (auto &e : l.list) {
            if (E()(e.first, key)) {
                return true;
            }
        }
        return false;
    }

    bool tryGet(K key, V &item) {
        V *found = nullptr;
        auto res = try_get(key, &found);
        if (res) {
            item = *found;
        }
        return res;
    }

    bool tryPop(std::pair<K, V> &item) {
        for (size_t i = 0; i < N; i++) {
            collision_list &l = collision_lists[i];
            l.mutex.lock_shared();
            if (l.list.empty()) {
                l.mutex.unlock_shared();
                continue;
            }
            l.mutex.unlock_shared();
            write_lock lock(l.mutex);
            if (l.list.empty()) {
                continue;
            }
            item = std::move(l.list.front());
            l.list.pop_front();
            return true;
        }
        return false;
    }

    V &getOrCreate(K key, std::function<V(K key)> factory) {
        V *f;
        if (try_get(key, &f)) {
            return *f;
        } else {
            V item = std::move(factory(key));
            auto &l = get_list(key);
            std::unique_lock<std::shared_mutex> lock(l.mutex);
            l.list.push_back(std::make_pair(key, std::move(item)));
            return l.list.back().second;
        }
    }

    bool tryCreate(K key, std::function<V(K const &key)> factory) {
        if (has(key)) {
            return false;
        } else {
            V item = factory(key);
            auto &l = get_list(key);
            std::unique_lock<std::shared_mutex> lock(l.mutex);
            l.list.push_back(std::make_pair(key, std::move(item)));
            return true;
        }
    }

    bool tryRemove(K key) {
        auto &l = get_list(key);
        write_lock lock(l.mutex);
        for (auto it = l.list.begin(); it != l.list.end(); it++) {
            if (E()(it->first, key)) {
                l.list.erase(it);
                return true;
            }
        }
        return false;
    }

    bool tryRemove(K key, V &item) {
        auto &l = get_list(key);
        write_lock lock(l.mutex);
        for (auto it = l.list.begin(); it != l.list.end(); it++) {
            if (E()(it->first, key)) {
                item = std::move(it->second);
                l.list.erase(it);
                return true;
            }
        }
        return false;
    }

    std::vector<K> getKeys() {
        std::vector<K> keys;
        for (size_t i = 0; i < N; i++) {
            collision_list &l = collision_lists[i];
            read_lock lock(l.mutex);
            for (auto &e : l.list) {
                keys.push_back(e.first);
            }
        }
        return keys;
    }

    bool empty() {
        for (size_t i = 0; i < N; i++) {
            collision_list &l = collision_lists[i];
            read_lock lock(l.mutex);
            if (!l.list.empty()) {
                return false;
            }
        }
        return true;
    }

    size_t count() {
        size_t c = 0;
        for (size_t i = 0; i < N; i++) {
            collision_list &l = collision_lists[i];
            read_lock lock(l.mutex);
            c += l.list.size();
        }
        return c;
    }

    void clear() {
        for (size_t i = 0; i < N; i++) {
            collision_list &l = collision_lists[i];
            write_lock lock(l.mutex);
            l.list.clear();
        }
    }

  private:
    collision_list &get_list(K key) {
        auto hash = H()(key) % N;
        return collision_lists[hash];
    }

    bool try_get(K key, V **item) {
        auto &l = get_list(key);
        read_lock lock(l.mutex);
        for (auto &e : l.list) {
            if (E()(e.first, key)) {
                *item = &e.second;
                return true;
            }
        }
        return false;
    }
};

NSP_UTIL_END