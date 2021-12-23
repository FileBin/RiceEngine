#pragma once 

#include "Voxel.h"

struct VoxelVoid : public Voxel {
    VoxelVoid(Vector3i pos) {
        position = pos;
        isVoid = true;
    }
};