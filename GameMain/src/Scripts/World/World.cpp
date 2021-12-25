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
                SetVoxel(&GenVoxel(p), p);
            }
        }
    }
}

Voxel& Chunk::GenVoxel(Vector3i voxelPos, bool genNormals) {
    auto t = voxelPos + position * ChunkSize;
    auto altitude = t.y - map->Get(voxelPos.x, voxelPos.z);
    return gen->GetVoxel(t, altitude);
}

Model* Chunk::GenerateModel() {
    auto mod = new Model();

    auto mesh = new Mesh();

    mesh->layout = Vertex::GetLayout();

    std::function<void(int, int, int)> func = [&](int x, int y, int z) {
        bool v1, v2;
        Vector3i inChunkPos{ x,y,z };
        Vector3i offset = position * ChunkSize;
        Vector3i worldPos = inChunkPos + offset;

        if (x < 0 || y < 0 || z < 0) {
            v1 = world->IsVoxelVoid(worldPos);
        } else {
            v1 = IsVoxelVoid(inChunkPos);
        }

        for (int i = 0; i < 3; i++) {
            Vector3i pos = inChunkPos;
            pos[i] += 1;

            if (pos[i] >= ChunkSize) {
                Vector3i wp = pos + offset;
                v2 = world->IsVoxelVoid(wp);
            } else {
                v2 = IsVoxelVoid(pos);
            }

            auto o = Vector3::one * .5f;

            auto angle = -90.;

            if (v1) {
                o[i] = 1.5f;
                if (v2) {
                    continue;
                }
            } else if (v2) {
                angle = 90;
            } else {
                continue;
            }
            auto c = Mesh::quad;
            c.Translate({ 0,0, .5 });
            Vector3 p = inChunkPos;
            p += o;
            //auto q = Quaternion::LookAt((Vector3)full->position, (Vector3)empty->position);
            auto q = Quaternion::identity;
            if (i == 0) {
                q = Quaternion::FromAxisAngle(Vector3::up, angle);
            } else if (i == 1) {
                q = Quaternion::FromAxisAngle(Vector3::left, angle);
            } else if (angle < 0) {
                q = Quaternion::FromAxisAngle(Vector3::left, 180);
            }
            c.Rotate(q);
            c.Translate(p);
            mesh->Combine(c);
        }
    };
    auto a = ChunkSize;
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < a; j++) {
            for (int k = 0; k < a; k++) {
                func(i, j, k);
            }
        }
    }

    mod->SetSubMeshesCount(1);
    mod->SetSubMesh(mesh, 0);
    return mod;
}

#include "Voxels\VoxelVoid.h"
#include "Voxels\VoxelGrass.h"

concurrency::concurrent_unordered_map <uint, std::function<Voxel* (VoxelData&, Vector3i&)>> Voxel::builders{};
concurrency::concurrent_unordered_map <uint, bool> Voxel::voidMap{};

void Voxel::Register() {
    builders.insert({ VoxelVoid::GetIdx(), VoxelVoid::Build });
    builders.insert({ VoxelGrass::GetIdx(), VoxelGrass::Build });

    voidMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsVoid() });
    voidMap.insert({ VoxelGrass::GetIdx(), VoxelGrass::IsVoid() });
}