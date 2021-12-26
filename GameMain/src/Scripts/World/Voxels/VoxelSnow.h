#pragma once 

#include "Voxel.h"

struct VoxelSnow : public Voxel {
    VoxelSnow(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return 4; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelSnow* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelSnow(pos);
    }
};