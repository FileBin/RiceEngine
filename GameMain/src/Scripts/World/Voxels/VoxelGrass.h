﻿#pragma once 

#include "Voxel.h"
#include <GameEngine\Engine.h>
#include <GameEngine\SceneRender.h>

struct VoxelGrass : public Voxel {
    VoxelGrass(Vector3i pos) {
        position = pos;
        isVoid = false;
        isTransparent = false;
    }
    static bool IsVoid() { return false; }
    static bool IsTransparent() { return false; }
    static uint GetIdx() { return VoxelTypeIndex::V_GRASS; }
    VoxelData GetData() { return { GetIdx() }; }
    static VoxelGrass* Build(VoxelData& data, Vector3i& pos) {
        return new VoxelGrass(pos);
    }

    static Material& CreateMaterial(SceneRender& ren) {
        auto& shader = ren.GetShader(L"Diffuse");
        auto& mat = ren.CreateMaterial(L"Grass", &shader, { Var(L"time"), Var(L"color") });
        mat.SetVar<Vector4f>(L"color", { 0.2f, 0.4f, 0.2f, 1.f });
        mat.UpdateBuffer();
        return mat;
    }
};