#pragma once

#include "cds/container/impl/iterable_list.h"
#include <bits/stdc++.h>
#include <memory>
#include <string>
#ifndef NDEBUG
#ifndef _DEBUG
#define _DEBUG
#endif //_DEBUG
#endif // NDEBUG

#include <cds/container/cuckoo_map.h>
#include <cds/container/cuckoo_set.h>
#include <cds/container/iterable_list_hp.h>
#include <cds/container/moir_queue.h>

template <typename T, typename R> using pair = std::pair<T, R>;

template <typename T> using vec = std::vector<T>;

template <typename K, typename V, typename C = std::less<K>,
          typename A = std::allocator<pair<const K, V>>>
using map = std::map<K, V, C, A>;

template <typename K, typename V, typename H = std::hash<K>, typename P = std::equal_to<K>,
          typename A = std::allocator<pair<const K, V>>>
using umap = std::unordered_map<K, V, H, P, A>;

template <typename T> using set = std::set<T>;
template <typename T> using uset = std::unordered_set<T>;

template <typename K, typename V> using concurent_map = cds::container::CuckooMap<K, V>;
template <typename T> using concurent_set = cds::container::CuckooSet<T>;
template <typename T> using concurent_queue = cds::container::MoirQueue<cds::gc::HP, T>;
template <typename T> using concurent_list = cds::container::IterableList<cds::gc::HP, T>;

template <typename T> using ptr = std::shared_ptr<T>;
template <typename T> using wptr = std::weak_ptr<T>;
template <typename T, typename D = std::default_delete<T>> using uptr = std::unique_ptr<T, D>;
template <typename T> using enable_ptr = std::enable_shared_from_this<T>;
typedef uint8_t byte;

typedef std::basic_string<byte> data_t;

typedef int64_t num;
typedef double dbl;
typedef uint32_t index_t;

template <typename T, typename... Args> inline ptr<T> new_ptr(Args &&...args) {
    return ptr<T>(new T(args...));
}
