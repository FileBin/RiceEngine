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

    auto nMeshes = Voxel::GetMaterialCount();

    auto meshes = std::vector<Mesh*>(nMeshes);

    for (auto& m : meshes) {
        m = new Mesh();
        m->layout = Vertex::GetLayout();
    }

    std::function<void(int, int, int)> func = [&](int x, int y, int z) {
        bool v1, v2;
        size_t idx1 = UINT32_MAX, idx2 = UINT32_MAX;
        Vector3i inChunkPos{ x,y,z };
        Vector3i offset = position * ChunkSize;
        Vector3i worldPos = inChunkPos + offset;

        if (x < 0 || y < 0 || z < 0) {
            idx1 = world->GetVoxelData(worldPos).index;
        } else {
            idx1 = GetData(inChunkPos).index;
        }

        v1 = Voxel::IsVoid(idx1);

        for (int i = 0; i < 3; i++) {
            Vector3i pos = inChunkPos;
            pos[i] += 1;

            if (pos[i] >= ChunkSize) {
                Vector3i wp = pos + offset;
                idx2 = world->GetVoxelData(wp).index;
            } else {
                idx2 = GetData(pos).index;
            }

            v2 = Voxel::IsVoid(idx2);

            auto o = Vector3::one * .5f;

            auto angle = -90.;

            size_t matIdx = UINT32_MAX;

            if (v1) {
                o[i] = 1.5f;
                if (v2) {
                    continue;
                } else {
                    matIdx = idx2;
                }
            } else if (v2) {
                matIdx = idx1;
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
            meshes[matIdx]->Combine(c);
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

    mod->SetSubMeshesCount(nMeshes);
    for (size_t i = 0; i < nMeshes;i++) {
        mod->SetSubMesh(meshes[i], i);
    }
    return mod;
}

#include "Voxels\VoxelVoid.h"
#include "Voxels\VoxelGrass.h"
#include "Voxels\VoxelDirt.h"
#include "Voxels\VoxelStone.h"
#include "Voxels\VoxelSnow.h"

concurrent_unordered_map <uint, std::function<Voxel* (VoxelData&, Vector3i&)>> Voxel::builders{};
concurrent_unordered_map <uint, bool> Voxel::voidMap{};
concurrent_unordered_map <uint, Material*> Voxel::materialMap{};

void Voxel::Register(Engine& en, SceneRender& ren) {
    builders.insert({ VoxelVoid::GetIdx(), VoxelVoid::Build });
    builders.insert({ VoxelGrass::GetIdx(), VoxelGrass::Build });
    builders.insert({ VoxelDirt::GetIdx(), VoxelDirt::Build });
    builders.insert({ VoxelStone::GetIdx(), VoxelStone::Build });
    builders.insert({ VoxelSnow::GetIdx(), VoxelSnow::Build });

    voidMap.insert({ VoxelVoid::GetIdx(), VoxelVoid::IsVoid() });
    voidMap.insert({ VoxelGrass::GetIdx(), VoxelGrass::IsVoid() });
    voidMap.insert({ VoxelDirt::GetIdx(), VoxelDirt::IsVoid() });
    voidMap.insert({ VoxelStone::GetIdx(), VoxelStone::IsVoid() });
    voidMap.insert({ VoxelSnow::GetIdx(), VoxelSnow::IsVoid() });

    materialMap.insert({ VoxelVoid::GetIdx(), nullptr });
    materialMap.insert({ VoxelGrass::GetIdx(), &VoxelGrass::CreateMaterial(en,ren) });
}