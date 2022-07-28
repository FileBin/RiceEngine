#pragma once

#include "bits/stdc++.h"
#ifndef NDEBUG
#ifndef _DEBUG
#define _DEBUG
#endif //_DEBUG
#endif // NDEBUG

template <typename T, typename R> using pair = std::pair<T, R>;

template <typename T> using vec = std::vector<T>;

template <typename K, typename V, typename C = std::less<K>,
          typename A = std::allocator<pair<const K, V>>>
using map = std::map<K, V, C, A>;

template <typename K, typename V, typename H = std::hash<K>,
          typename P = std::equal_to<K>,
          typename A = std::allocator<pair<const K, V>>>
using umap = std::unordered_map<K, V, H, P, A>;

template <typename T> using set = std::set<T>;

template <typename T> using uset = std::unordered_set<T>;

template <typename T> using ptr = std::shared_ptr<T>;
template <typename T> using wptr = std::weak_ptr<T>;
template <typename T> using uptr = std::unique_ptr<T>;
template <typename T> using enable_ptr = std::enable_shared_from_this<T>;
typedef uint8_t byte;

typedef vec<byte> data_t;

typedef int64_t num;
typedef double dbl;
