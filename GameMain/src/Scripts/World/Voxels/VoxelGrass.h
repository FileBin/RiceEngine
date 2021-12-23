#pragma once 

#include "Voxel.h"

struct VoxelGrass : public Voxel {
    VoxelGrass(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
};