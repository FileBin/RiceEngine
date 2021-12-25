#pragma once 

#include "Voxel.h"

struct VoxelGrass : public Voxel {
    VoxelGrass(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return 1; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelGrass(pos);
    }
};