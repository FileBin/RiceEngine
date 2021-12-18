#pragma once

#include <GameEngine\Vectors.h>

struct Voxel {
	Vector3i position;
	bool isVoid = false;
};
