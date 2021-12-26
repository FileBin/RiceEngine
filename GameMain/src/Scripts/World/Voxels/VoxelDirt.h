#pragma once 

#include "Voxel.h"

struct VoxelDirt : public Voxel {
    VoxelDirt(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return 2; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDirt* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDirt(pos);
    }
};