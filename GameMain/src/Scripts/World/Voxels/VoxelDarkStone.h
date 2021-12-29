#pragma once 

#include "Voxel.h"
#include <GameEngine\Engine.h>
#include <GameEngine\SceneRender.h>

struct VoxelDarkStone : public Voxel {
    VoxelDarkStone(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static const Vector3f EGS;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DARK_STONE; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDarkStone* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDarkStone(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"DarkStone", &shader, { Var(L"time"), Var(L"color"), Var(L"egs") });
        mat.SetVar<Vector4f>(L"color", color);
        mat.SetVar<Vector3f>(L"egs", EGS);
        mat.UpdateBuffer();
        return mat;
    }
};