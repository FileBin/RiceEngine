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
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return true; }
    static uint GetIdx() { return VoxelTypeIndex::V_WATER; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelWater* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelWater(pos);
    }

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"water.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        auto& mat = ren.CreateMaterial(L"Water", &shader);
        mat.renderType = RenderType::Transparent;
        return mat;
    }
};