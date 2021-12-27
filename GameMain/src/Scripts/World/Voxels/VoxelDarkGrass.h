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
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DARK_GRASS; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDarkGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDarkGrass(pos);
    }

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"dark_grass.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        return ren.CreateMaterial(&shader);
    }
};