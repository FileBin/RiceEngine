#pragma once
#include <xkeycheck.h>

struct Vector3i {
public:
	num x, y, z;
	Vector3i() = default;
	Vector3i(num x, num y, num z) : x(x), y(y), z(z) {}
	Vector3i(const Vector3& vec);
	long long& operator[](size_t idx);
};

bool operator==(Vector3i a, Vector3i b);

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