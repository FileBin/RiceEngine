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
    static const Vector4f EGST;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DARK_GRASS; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDarkGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDarkGrass(pos);
    }

    static std::shared_ptr<Material> CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto mat = ren.CreateMaterial(L"DarkGrass", &shader, { Var(L"time"), Var(L"color"), Var(L"egst") });
        mat->SetVar<Vector4f>(L"color", color);
        mat->SetVar<Vector4f>(L"egst", EGST);
        mat->UpdateBuffer();
        return mat;
    }
};