#pragma once 

#include "Voxel.h"

struct VoxelSnow : public Voxel {
    VoxelSnow(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static const Vector4f EGST;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_SNOW; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelSnow* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelSnow(pos);
    }

    static SmartPtr<Material> CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto mat = ren.CreateMaterial(L"Snow", &shader, { Var(L"time"), Var(L"color"), Var(L"egst")});
        mat->SetVar<Vector4f>(L"color", color);
        mat->SetVar<Vector4f>(L"egst", EGST);
        mat->UpdateBuffer();
        return mat;
    }
};