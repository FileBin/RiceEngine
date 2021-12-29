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
    static const Vector3f EGS;
    static const Vector4f color;
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return true; }
    static uint GetIdx() { return VoxelTypeIndex::V_WATER; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelWater* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelWater(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        //shader.LoadVertexShader(Util::ReadFile(L"water.cso"), Vertex::GetLayout());
        //shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

        auto& mat = ren.CreateMaterial(L"Water", &shader, { Var(L"time"), Var(L"color"), Var(L"egs") });
        mat.renderType = RenderType::Transparent;
        mat.SetVar<Vector4f>(L"color", color);
        mat.SetVar<Vector3f>(L"egs", EGS);
        mat.UpdateBuffer();
        return mat;
    }
};