#pragma once
#include <GameEngine\Vectors.h>

#include "Voxel.h"

class WorldGenerator {
public:
	virtual double GetTerrainHeight(Vector2 pos) = 0;
	virtual Voxel& GetVoxel(Vector3 pos, double groundAltitude) = 0;
};
