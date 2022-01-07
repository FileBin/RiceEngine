#include "pch.h"

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
    if(hmap.IsNull()) {
        hmap = world->GetHeightMap(position);
    }
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
#include "Voxels\VoxelSand.h"
#include "Voxels\VoxelWater.h"

concurrent_unordered_map <uint, std::function<Voxel* (VoxelData&, Vector3i&)>> Voxel::builders{};
concurrent_unordered_map <uint, bool> Voxel::voidMap{};
concurrent_unordered_map <uint, bool> Voxel::transparentMap{};
concurrent_unordered_map <uint, SmartPtr<Material>> Voxel::materialMap{};

void Voxel::Register(SceneRender& ren) {

    builders.insert({ VoxelVoid::GetIdx(), VoxelVoid::Build });
    voidMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsVoid() });
    transparentMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsTransparent() });
    materialMap.insert({ VoxelVoid::GetIdx(), nullptr });

    registerVoxel<VoxelGrass>(ren);
    registerVoxel<VoxelDirt>(ren);
    registerVoxel<VoxelStone>(ren);
    registerVoxel<VoxelSnow>(ren);
    registerVoxel<VoxelDarkStone>(ren);
    registerVoxel<VoxelDarkGrass>(ren);
    registerVoxel<VoxelSand>(ren);
    registerVoxel<VoxelWater>(ren);
}