#pragma once
#include <GameEngine/Vectors.h>

#include "Voxels/Voxel.h"

class WorldGenerator {
public:
	virtual dbl GetTerrainHeight(Vector2 pos) = 0;
	virtual VoxelData GetVoxelData(Vector3 pos, dbl groundAltitude) = 0;
};
