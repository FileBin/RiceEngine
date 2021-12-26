#pragma once
#include <xkeycheck.h>
#include <functional>

#include "../Util/defines.h"
#include "Vector3i.h"

template <> struct std::hash<Vector3i> {
	size_t operator()(const Vector3i& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y) ^ hash<num>()(a.z);
	}
};

template <> struct std::equal_to<Vector3i> {
	bool operator()(const Vector3i& a, const Vector3i& b) const {
		return a == b;
	}
};