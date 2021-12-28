#pragma once 

#include "Voxel.h"

struct VoxelStone : public Voxel {
    VoxelStone(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_STONE; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelStone* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelStone(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Stone", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 0.3f, 0.3f, 0.3f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};