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
    VoxelData GetVoxelData(Vector3 pos, dbl groundAltitude, bool transparancy){
        float d = (float)groundAltitude;
        if(groundAltitude > 0)
            return { VoxelTypeIndex::V_VOID, d };
        return { VoxelTypeIndex::V_GRASS, d };
    }
};