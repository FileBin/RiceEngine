#pragma once 

#include "Voxel.h"
#include <GameEngine\Engine.h>
#include <GameEngine\SceneRender.h>

struct VoxelDarkGrass : public Voxel {
    VoxelDarkGrass(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static const Vector3f EGS;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DARK_GRASS; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDarkGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDarkGrass(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"DarkGrass", &shader, { Var(L"time"), Var(L"color"), Var(L"egs") });
        mat.SetVar<Vector4f>(L"color", color);
        mat.SetVar<Vector3f>(L"egs", EGS);
        mat.UpdateBuffer();
        return mat;
    }
};