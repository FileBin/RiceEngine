#pragma once
#include "../Util.h"

struct Vector2i {
	num x, y;
};

bool operator==(Vector2i a, Vector2i b);

template <> struct std::hash<Vector2i> {
	size_t operator()(const Vector2i& a) const {
		return hash<dbl>()(a.x) ^ hash<dbl>()(a.y);
	}
};

template <> struct std::equal_to<Vector2i> {
	bool operator()(const Vector2i& a, const Vector2i& b) const {
		return std::equal_to<dbl>()(a.x, b.x) && std::equal_to<dbl>()(a.y, b.y);
	}
};