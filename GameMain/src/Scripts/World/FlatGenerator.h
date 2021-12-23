#pragma once

#include "WorldGenerator.h"
#include "Voxels/VoxelGrass.h"
#include "Voxels/VoxelVoid.h"

class FlatGenerator : public WorldGenerator {
private:
    dbl terrainHeight = 0;
public:

    FlatGenerator(dbl terrainHeight) : terrainHeight(terrainHeight) {}
    dbl GetTerrainHeight(Vector2 pos){
        return terrainHeight;
    }
    Voxel& GetVoxel(Vector3 pos, dbl groundAltitude){
        if(groundAltitude > 0)
            return *(new VoxelVoid(pos));
        return *(new VoxelGrass(pos));
    }
};