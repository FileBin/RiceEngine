#include "HeightMap.h"
#include "Chunk.h"

HeightMap::HeightMap(WorldGenerator& worldGenerator, Vector2i pos, bool generateNormals) {
    auto n = Chunk::ChunkSize;
    map.resize(n * n);
    //normalMap = new NormalMap();
    minh = LLONG_MAX;
    maxh = LLONG_MIN;
    if (!generateNormals) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Vector2 terrainPos{ (let)(pos.x * n + i), (let)(pos.y * n + j) };
                let h = worldGenerator.GetTerrainHeight(terrainPos);
                minh = min(h, minh);
                maxh = max(h, maxh);
                map[i, j] = h;
                //normalMap.SetNormal(i, j, Vector3.up);
            }
        }
    }
}

let HeightMap::Get(int x, int y) {
    return map[x * Chunk::ChunkSize + y];
}


void Chunk::Generate(WorldGenerator& gen, Vector3i pos, HeightMap& map) {
    auto length = ChunkSize;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            for (int k = 0; k < length; k++) {
                Vector3i p { i,j,k };
                SetVoxel(&GenVoxel(p), p);
            }
        }
    }
}

Voxel& Chunk::GenVoxel(Vector3i voxelPos, bool genNormals) {
    auto t = voxelPos + pos * ChunkSize;
    auto altitude = t.y - map->Get(voxelPos.x, voxelPos.z);
    return gen->GetVoxel(t, altitude);
}