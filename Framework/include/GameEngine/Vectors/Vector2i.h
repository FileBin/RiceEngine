#pragma once
#include "../Util.h"

struct Vector2i {
	num x, y;
};

bool operator==(Vector2i a, Vector2i b) {
	return (a.x == b.x) && (a.y == b.y);
}