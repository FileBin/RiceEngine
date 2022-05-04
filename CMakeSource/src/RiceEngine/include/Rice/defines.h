#pragma once


#ifndef NDEBUG
#ifndef _DEBUG
#define _DEBUG
#endif //_DEBUG
#endif //NDEBUG

#ifndef _MSC_VER
#define _STD ::std::
#endif

#include "stdafx.hpp"

template <typename T>
using vec = _STD vector<T>;

template <typename K, typename V, typename C = _STD less<K>, typename A = _STD allocator<_STD pair<const K, V> >>
using map = _STD map<K,V,C,A>;

template <typename K, typename V, typename H = _STD hash<K>, typename P = _STD equal_to<K>, typename A = _STD allocator<_STD pair<const K, V> >>
using umap = _STD unordered_map<K,V,H,P,A>;

template <typename T>
using set = _STD set<T>;

template <typename T>
using uset = _STD unordered_set<T>;

typedef vec<byte> data_t;

typedef long long num;
typedef double dbl;

struct Vector3;

typedef _STD function<dbl(Vector3)> SDFunc;

typedef bettercpp::RefPtr<SDL_Window> WindowHandle;
