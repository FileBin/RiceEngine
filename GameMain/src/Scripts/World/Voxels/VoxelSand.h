#pragma once 

#include "Voxel.h"

struct VoxelSand : public Voxel {
    VoxelSand(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_SAND; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelSand* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelSand(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Sand", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 0.88f, 0.74f, 0.4f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};