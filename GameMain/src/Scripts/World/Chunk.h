#pragma once
#include <vector>
#include <GameEngine/Model.h>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include "Voxels/Voxel.h"
#include "WorldGenerator.h"
#include <GameEngine/Vectors/Hasher.h>

using namespace Game;
using std::map, std::unordered_map;

class HeightMap;
class World;

class Chunk {
#define LOD_COUNT 4
private:
    WorldGenerator* gen;
    HeightMap* hmap;
    std::shared_ptr<Model>* model[LOD_COUNT] = { nullptr, nullptr, nullptr, nullptr };
    World* world;

    bool lock = false;

    std::vector<VoxelData> voxels{};
    map<size_t, unordered_map<Vector3i, float>> transpDepth{};
    void Generate();

public:
    enum Status { Invalid = -3, Unloaded, NotCreated, NotLoaded = 0, Loading, Operating, Loaded } status = NotLoaded;

    static const int ChunkSize;
    Vector3i position{};

    Chunk(WorldGenerator* gen, Vector3i pos, HeightMap* _map, World* world) {
        voxels.resize((INT64)ChunkSize * ChunkSize * ChunkSize);
        this->gen = gen;
        this->position = pos;
        this->hmap = _map;
        this->world = world;
    }

    ~Chunk() {
        voxels.clear();
        for (int i = 0; i < LOD_COUNT; i++) {
            delete model[i];
        }
    }

    bool IsVoxelVoid(Vector3i voxelPos) {
        return Voxel::IsVoid(GetData(voxelPos).index);
    }

    bool IsVoxelVoid(VoxelData data) {
        return Voxel::IsVoid(data.index);
    }

    float GetTranspDepth(Vector3i pos, size_t index) {
        auto it = transpDepth.find(index);
        if (it != transpDepth.end()) {
            auto mp = it->second;
            auto it2 = mp.find(pos);
            if (it2 != mp.end()) {
                return it2->second;
            }
        }
        throw new Game::exception("TransparentVoxel not found", 71, L"Chunk.h : float GetTranspDepth(Vector3i pos, size_t index)");
    }

    VoxelData GetData(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        while (lock) Sleep(1);
        lock = true;
        auto& data = voxels[idx];
        if (data.index == UINT32_MAX) {
            data = GenVoxelData(pos);
            if (Voxel::IsTransparent(data.index)) {
                auto transpData = GenVoxelData(pos, true);
                SetTranspData(transpData, pos);
            }
            lock = false;
            SetVoxelData(data, pos);
        }
        lock = false;
        return data;
    }

    Voxel& GetVoxel(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        while (lock) Sleep(1);
       // lock = true;
        Voxel* vox = nullptr;
        if (voxels[idx].index == UINT32_MAX) {
           vox = Voxel::Build(GetData(pos), pos);
           // lock = false;
            //SetVoxel(vox, pos);
        } else {
            vox = Voxel::Build(voxels[idx], pos);
        }
        lock = false;
        return *vox;
    }

    VoxelData GenVoxelData(Vector3i voxelPos, bool transparency = false);

    void SetVoxel(Voxel* vox, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;

        while (lock) Sleep(1);
        lock = true;

        voxels[(x * ChunkSize + y) * ChunkSize + z] = vox->GetData();

        lock = false;
    }

    void SetVoxelData(VoxelData data, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;

        while (lock) Sleep(1);
        lock = true;

        voxels[(x * ChunkSize + y) * ChunkSize + z] = data;

        lock = false;
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

    std::shared_ptr<Model> GetModel(size_t lod = 0) {
        auto idx = 1 << lod;
        if (model[lod]) return *model[lod];
        return *(model[lod] = new std::shared_ptr<Model>(GenerateSmoothModel(idx)));
    }
    Model* GenerateModel();
    Model* GenerateSmoothModel(size_t step = 4);
};
