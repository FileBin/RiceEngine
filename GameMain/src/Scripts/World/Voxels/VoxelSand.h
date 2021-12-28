#pragma once 

#include "Voxel.h"

struct VoxelSand : public Voxel {
    VoxelSand(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_SAND; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelSand* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelSand(pos);
    }

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"sand.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        return ren.CreateMaterial(&shader);
    }
};