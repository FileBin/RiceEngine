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

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"dark_stone.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        return ren.CreateMaterial(&shader);
    }
};