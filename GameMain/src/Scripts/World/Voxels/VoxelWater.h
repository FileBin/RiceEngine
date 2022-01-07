#pragma once 

#include "Voxel.h"
#include <GameEngine\Engine.h>
#include <GameEngine\SceneRender.h>

struct VoxelWater : public Voxel {
    VoxelWater(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = true;
    }
    static const Vector4f EGST;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return true; }
    static uint GetIdx() { return VoxelTypeIndex::V_WATER; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelWater* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelWater(pos);
    }

    static SmartPtr<Material> CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Water");

        auto mat = ren.CreateMaterial(L"Water", &shader, { Var(L"time"), Var(L"resolution") });
        mat->AddTexture(&ren.GetDepthBufferTex());
        mat->renderType = RenderType::Transparent;
        mat->UpdateBuffer();
        return mat;
    }
};