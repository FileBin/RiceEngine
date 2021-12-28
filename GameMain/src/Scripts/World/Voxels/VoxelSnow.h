#pragma once 

#include "Voxel.h"

struct VoxelSnow : public Voxel {
    VoxelSnow(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_SNOW; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelSnow* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelSnow(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Snow", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 1.f, 1.f, 1.f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};