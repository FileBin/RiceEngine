#pragma once 

#include "Voxel.h"
#include <GameEngine\Engine.h>
#include <GameEngine\SceneRender.h>

struct VoxelGrass : public Voxel {
    VoxelGrass(Vector3i pos) {
        position = pos;
        isVoid = false;
    }
    static bool IsVoid() { return false; }
    static uint GetIdx() { return 1; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelGrass(pos);
    }

    static Material& CreateMaterial(Engine& en, SceneRender& ren) {
        auto& shader = en.CreateShader();
        shader.LoadVertexShader(Util::ReadFile(L"grass.cso"), Vertex::GetLayout());
        shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        return ren.CreateMaterial(&shader);
    }
};