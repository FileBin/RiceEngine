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
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DARK_STONE; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDarkStone* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDarkStone(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"DarkStone", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 0.2f, 0.2f, 0.2f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};