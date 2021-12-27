#pragma once 

#include "Voxel.h"

struct VoxelVoid : public Voxel {
    VoxelVoid(Vector3i pos) {
        position = pos;
        isVoid = true;
        isTransparent = true;
    }
    static bool IsVoid() { return true; }
    static bool IsTransparent() { return true; }
    static uint GetIdx() { return VoxelTypeIndex::V_VOID; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelVoid* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelVoid(pos);
    }
};