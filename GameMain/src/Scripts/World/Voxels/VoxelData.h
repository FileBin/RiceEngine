#pragma once
#include <GameEngine\Util\defines.h>

struct VoxelData {
	float depth = 0.f;
	uint index = UINT32_MAX;

	VoxelData(uint idx) {
		index = idx;
	}

	VoxelData(uint idx, float d) {
		index = idx;
		depth = d;
	}

	VoxelData() = default;
};
