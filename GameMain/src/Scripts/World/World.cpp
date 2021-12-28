#include "HeightMap.h"
#include "Chunk.h"
#include "World.h"

HeightMap::HeightMap(WorldGenerator& worldGenerator, Vector2i pos, bool generateNormals) {
    auto n = Chunk::ChunkSize;
    map.resize((num)n * n);
    //normalMap = new NormalMap();
    minh = LLONG_MAX;
    maxh = LLONG_MIN;
    if (!generateNormals) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Vector2 terrainPos{ (dbl)(pos.x * n + i), (dbl)(pos.y * n + j) };
                dbl h = worldGenerator.GetTerrainHeight(terrainPos);
                minh = min(h, minh);
                maxh = max(h, maxh);
                map[i*n+j] = h;
                //normalMap.SetNormal(i, j, Vector3.up);
            }
        }
    }
}

dbl HeightMap::Get(int x, int y) {
    return map[(num)x * Chunk::ChunkSize + y];
}


void Chunk::Generate() {
    auto length = ChunkSize;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            for (int k = 0; k < length; k++) {
                Vector3i p { i,j,k };
                SetVoxelData(GenVoxelData(p), p);
            }
        }
    }
}

VoxelData Chunk::GenVoxelData(Vector3i voxelPos, bool transp) {
    auto t = voxelPos + position * ChunkSize;
    auto altitude = t.y - hmap->Get(voxelPos.x, voxelPos.z);
    return gen->GetVoxelData(t, altitude, transp);
}

#include "Voxels\VoxelVoid.h"
#include "Voxels\VoxelGrass.h"
#include "Voxels\VoxelDirt.h"
#include "Voxels\VoxelStone.h"
#include "Voxels\VoxelSnow.h"
#include "Voxels\VoxelDarkStone.h"
#include "Voxels\VoxelDarkGrass.h"
#include "Voxels\VoxelWater.h"

concurrent_unordered_map <uint, std::function<Voxel* (VoxelData&, Vector3i&)>> Voxel::builders{};
concurrent_unordered_map <uint, bool> Voxel::voidMap{};
concurrent_unordered_map <uint, bool> Voxel::transparentMap{};
concurrent_unordered_map <uint, Material*> Voxel::materialMap{};

void Voxel::Register(SceneRender& ren) {
    builders.insert({ VoxelVoid::GetIdx(), VoxelVoid::Build });
    builders.insert({ VoxelGrass::GetIdx(), VoxelGrass::Build });
    builders.insert({ VoxelDirt::GetIdx(), VoxelDirt::Build });
    builders.insert({ VoxelStone::GetIdx(), VoxelStone::Build });
    builders.insert({ VoxelSnow::GetIdx(), VoxelSnow::Build });
    builders.insert({ VoxelDarkStone::GetIdx(), VoxelDarkStone::Build });
    builders.insert({ VoxelDarkGrass::GetIdx(), VoxelDarkGrass::Build });
    builders.insert({ VoxelWater::GetIdx(), VoxelWater::Build });

    voidMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsVoid() });
    voidMap.insert({ VoxelGrass::GetIdx(), VoxelGrass::IsVoid() });
    voidMap.insert({ VoxelDirt::GetIdx(), VoxelDirt::IsVoid() });
    voidMap.insert({ VoxelStone::GetIdx(), VoxelStone::IsVoid() });
    voidMap.insert({ VoxelSnow::GetIdx(), VoxelSnow::IsVoid() });
    voidMap.insert({ VoxelDarkStone::GetIdx(), VoxelDarkStone::IsVoid() });
    voidMap.insert({ VoxelDarkGrass::GetIdx(), VoxelDarkGrass::IsVoid() });
    voidMap.insert({ VoxelWater::GetIdx(), VoxelWater::IsVoid() });

    transparentMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsTransparent() });
    transparentMap.insert({ VoxelGrass::GetIdx(), VoxelGrass::IsTransparent() });
    transparentMap.insert({ VoxelDirt::GetIdx(), VoxelDirt::IsTransparent() });
    transparentMap.insert({ VoxelStone::GetIdx(), VoxelStone::IsTransparent() });
    transparentMap.insert({ VoxelSnow::GetIdx(), VoxelSnow::IsTransparent() });
    transparentMap.insert({ VoxelDarkStone::GetIdx(), VoxelDarkStone::IsTransparent() });
    transparentMap.insert({ VoxelDarkGrass::GetIdx(), VoxelDarkGrass::IsTransparent() });
    transparentMap.insert({ VoxelWater::GetIdx(), VoxelWater::IsTransparent() });

    materialMap.insert({ VoxelVoid::GetIdx(), nullptr });
    materialMap.insert({ VoxelGrass::GetIdx(), &VoxelGrass::CreateMaterial(ren) });
    materialMap.insert({ VoxelDirt::GetIdx(), &VoxelDirt::CreateMaterial(ren) });
    materialMap.insert({ VoxelStone::GetIdx(), &VoxelStone::CreateMaterial(ren) });
    materialMap.insert({ VoxelSnow::GetIdx(), &VoxelSnow::CreateMaterial(ren) });
    materialMap.insert({ VoxelDarkStone::GetIdx(), &VoxelDarkStone::CreateMaterial(ren) });
    materialMap.insert({ VoxelDarkGrass::GetIdx(), &VoxelDarkGrass::CreateMaterial(ren) });
    materialMap.insert({ VoxelWater::GetIdx(), &VoxelWater::CreateMaterial(ren) });
}