#pragma once
#include <vector>
#include <GameEngine/Model.h>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <GameEngine\stdafx.h>
#include "Voxels/Voxel.h"
#include "WorldGenerator.h"
#include <GameEngine/Vectors/Hasher.h>
#include "HeightMap.h"
#include <GameEngine\Scene\SceneRender.h>
#include <GameEngine\Components\MeshCollider.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Components\MultiModelRender.h>

using namespace Game;
using std::map, std::unordered_map;

class World;

class Chunk {
#define LOD_COUNT 4
private:
    WorldGenerator* gen;
    std::vector<SmartPtr<Model>> model { LOD_COUNT };
    SmartPtr<HeightMap> hmap;
    World* world;

    std::shared_mutex lock;

    std::vector<VoxelData> voxels{};
    unordered_map<size_t, unordered_map<Vector3i, float>> transpDepth{};
    void Generate();

public:
#ifdef MULTI_RENDER
    MultiModelRender::UUID uuid;
#else
    SmartPtr<ModelRender> render;
#endif
    SmartPtr<MeshCollider> collider;

    enum Status { Invalid = -3, Unloaded, NotCreated, NotLoaded = 0, Loading, Operating, Loaded } status = NotLoaded;

    static const int ChunkSize;
    Vector3i position{};

    static long GetMaxLod() {
        return LOD_COUNT - 1;
    }

    static long GetMinLod() {
        return 0;
    }

    Chunk(WorldGenerator* gen, Vector3i pos, SmartPtr<HeightMap> _hmap, World* world) {
        voxels.resize((INT64)ChunkSize * ChunkSize * ChunkSize);
        this->gen = gen;
        hmap = _hmap;
        this->position = pos;
        this->world = world;
    }

    ~Chunk() {
        std::unique_lock l(lock);
       // hmap.Release();
        voxels.clear();
        //std::lock_guard guard(lock);
        for (auto it : model) {
            it.Release();
        }
        for (auto it : transpDepth) {
            it.second.clear();
        }
    }

    bool IsVoxelVoid(Vector3i voxelPos) {
        return Voxel::IsVoid(GetData(voxelPos).index);
    }

    bool IsVoxelVoid(VoxelData data) {
        return Voxel::IsVoid(data.index);
    }

    float GetTranspDepth(Vector3i pos, size_t index) {
        auto& mp = transpDepth[index];
        return mp[pos];
       /* auto it = transpDepth.find(index);
        if (it != transpDepth.end()) {
            auto mp = it->second;
            auto it2 = mp.find(pos);
            if (it2 != mp.end()) {
                return it2->second;
            }
        }*/
        THROW_EXCEPTION("TransparentVoxel not found");
    }

    VoxelData GetData(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        std::shared_lock guard(lock);
        auto& data = voxels[idx];
        if (data.index == UINT32_MAX) {
            voxels[idx] = GenVoxelData(pos);
            if (Voxel::IsTransparent(data.index)) {
                auto transpData = GenVoxelData(pos, true);
                SetTranspData(transpData, pos);
            }
            //SetVoxelData(data, pos);
        }
        return data;
    }

    Voxel& GetVoxel(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        std::shared_lock guard(lock);
       // lock = true;
        Voxel* vox = nullptr;
        if (voxels[idx].index == UINT32_MAX) {
           vox = Voxel::Build(GetData(pos), pos);
           // lock = false;
            //SetVoxel(vox, pos);
        } else {
            vox = Voxel::Build(voxels[idx], pos);
        }
        return *vox;
    }

    VoxelData GenVoxelData(Vector3i voxelPos, bool transparency = false);

    void SetVoxel(Voxel* vox, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;

        std::unique_lock guard(lock);

        voxels[(x * ChunkSize + y) * ChunkSize + z] = vox->GetData();
    }

    void SetVoxelData(VoxelData data, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;

        std::unique_lock guard(lock);

        voxels[(x * ChunkSize + y) * ChunkSize + z] = data;
    }

    void SetTranspData(VoxelData data, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;

        //while (lock) Sleep(1);
        //lock = true;

        auto pair = transpDepth.insert({ data.index, {} });
        unordered_map<Vector3i, float> *mp = nullptr;
        if (pair.second) { // if value added
            mp = &pair.first->second; // get map at iterator
        } else {
            mp = &transpDepth.at(data.index);
        }

        auto pair2 = mp->insert({ chunkPos, data.depth });

        if (!pair2.second) {
            pair2.first->second = data.depth; //set iterator value
        }

        //lock = false;
    }

    Voxel& GetVoxel(int x, int y, int z) {
        return GetVoxel({ x, y, z });
    }

    SmartPtr<Model> GetModel(size_t lod = 0) {
        auto idx = 1 << lod;
        if (model[lod].Get()) return { model[lod] };
        return model[lod] = GenerateSmoothModel(idx);
    }

    void UpdateModel(SceneRender& ren, size_t lod = 0);
    Model* GenerateModel();
    Model* GenerateSmoothModel(size_t step = 4);
};
