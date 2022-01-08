#pragma once

#include "../Util/defines.h"
#include "../Util/SmartPointer.h"

struct Vector2;
struct Vector2f;
struct Vector2i;

struct Vector3;
struct Vector3f;
struct Vector3i;

#pragma region Vector3_all
#pragma region Vector3

template <> struct std::hash<Vector3> {
	size_t operator()(const Vector3& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y) ^ hash<num>()(a.z);
	}
};

template <> struct std::equal_to<Vector3> {
	bool operator()(const Vector3& a, const Vector3& b) const {
		return a == b;
	}
};

#pragma endregion
#pragma region Vector3f

template <> struct std::hash<Vector3f> {
	size_t operator()(const Vector3f& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y) ^ hash<num>()(a.z);
	}
};

template <> struct std::equal_to<Vector3f> {
	bool operator()(const Vector3f& a, const Vector3f& b) const {
		return a == b;
	}
};

#pragma endregion
#pragma region Vector3i

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

#pragma endregion
#pragma endregion

#pragma region Vector2_all
#pragma region Vector2

template <> struct std::hash<Vector2> {
	size_t operator()(const Vector2& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y);
	}
};

template <> struct std::equal_to<Vector2> {
	bool operator()(const Vector2& a, const Vector2& b) const {
		return a == b;
	}
};

#pragma endregion
#pragma region Vector2f

template <> struct std::hash<Vector2f> {
	size_t operator()(const Vector2f& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y);
	}
};

template <> struct std::equal_to<Vector2f> {
	bool operator()(const Vector2f& a, const Vector2f& b) const {
		return a == b;
	}
};

#pragma endregion
#pragma region Vector2i

template <> struct std::hash<Vector2i> {
	size_t operator()(const Vector2i& a) const {
		return hash<num>()(a.x) ^ hash<num>()(a.y);
	}
};

template <> struct std::equal_to<Vector2i> {
	bool operator()(const Vector2i& a, const Vector2i& b) const {
		return a == b;
	}
};

#pragma endregion
#pragma endregion

#pragma region SmartPtr
template <typename T> struct std::hash<SmartPtr<T>> {
	size_t operator()(const SmartPtr<T>& a) const {
		return (size_t)std::hash<T*>()(a.Get());
	}
};
#pragma endregion