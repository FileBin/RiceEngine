#pragma once 

#include "Voxel.h"

struct VoxelDirt : public Voxel {
    VoxelDirt(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_DIRT; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelDirt* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelDirt(pos);
    }

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"dirt.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        return ren.CreateMaterial(&shader);
    }
};