#pragma once 

#include "Voxel.h"

struct VoxelDirt : public Voxel {
    VoxelDirt(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static const Vector3f EGS;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DIRT; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDirt* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDirt(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Dirt", &shader, { Var(L"time"), Var(L"color"), Var(L"egs") });
        mat.SetVar<Vector4f>(L"color", color);
        mat.SetVar<Vector3f>(L"egs", EGS);
        mat.UpdateBuffer();
        return mat;
    }
};