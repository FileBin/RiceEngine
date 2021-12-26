#pragma once 

#include "Voxel.h"

struct VoxelStone : public Voxel {
    VoxelStone(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return 3; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelStone* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelStone(pos);
    }
};