#include "Rice/Util/namespaces.h"
#include "stdafx.hpp"

NSP_UTIL_BEGIN
struct Rect;
NSP_UTIL_END

NSP_UTIL_BEGIN
struct Rect {
    int x, y, w, h;
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    Rect(const Rect& r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
    Rect(Rect&& r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
    Rect& operator=(const Rect& r) {
        x = r.x;
        y = r.y;
        w = r.w;
        h = r.h;
        return *this;
    }
    Rect& operator=(Rect&& r) {
        x = r.x;
        y = r.y;
        w = r.w;
        h = r.h;
        return *this;
    }
    bool operator==(const Rect& r) const {
        return x == r.x && y == r.y && w == r.w && h == r.h;
    }
    bool operator!=(const Rect& r) const {
        return !(*this == r);
    }
    bool operator<(const Rect& r) const {
        return x < r.x || (x == r.x && y < r.y) || (x == r.x && y == r.y && w < r.w) || (x == r.x && y == r.y && w == r.w && h < r.h);
    }
    bool operator>(const Rect& r) const {
        return x > r.x || (x == r.x && y > r.y) || (x == r.x && y == r.y && w > r.w) || (x == r.x && y == r.y && w == r.w && h > r.h);
    }
    bool operator<=(const Rect& r) const {
        return !(*this > r);
    }

    bool operator>=(const Rect& r) const {
        return !(*this < r);
    }
};
NSP_UTIL_END