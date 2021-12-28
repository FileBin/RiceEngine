#pragma once 

#include "Voxel.h"

struct VoxelDirt : public Voxel {
    VoxelDirt(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DIRT; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDirt* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDirt(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Dirt", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 0.2f, 0.1f, 0.f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};