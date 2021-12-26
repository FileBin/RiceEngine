#pragma once
#include <vector>
#include <GameEngine\Model.h>
#include <functional>
#include <vector>
#include "Voxels/Voxel.h"
#include "WorldGenerator.h"

using namespace Game;

class HeightMap;
class World;

class Chunk {
private:
    WorldGenerator* gen;
    HeightMap* map;
    Model* model = nullptr;
    World* world;

    bool lock = false;

    std::vector<VoxelData> voxels{};
    void Generate();

public:
    enum Status { Invalid = -3, Unloaded, NotCreated, NotLoaded = 0, Loading, Operating, Loaded } status = NotLoaded;

    static const int ChunkSize;
    Vector3i position{};

    Chunk(WorldGenerator* gen, Vector3i pos, HeightMap* map, World* world) {
        voxels.resize((INT64)ChunkSize * ChunkSize * ChunkSize);
        this->gen = gen;
        this->position = pos;
        this->map = map;
        this->world = world;
    }

    ~Chunk() {
        voxels.clear();
        if (model != nullptr) {
            delete model;
            model = nullptr;
        }
    }

    bool IsVoxelVoid(Vector3i voxelPos) {
        return Voxel::IsVoid(GetData(voxelPos).index);
    }

    bool IsVoxelVoid(VoxelData data) {
        return Voxel::IsVoid(data.index);
    }

    VoxelData GetData(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        while (lock) Sleep(1);
        lock = true;
        auto& data = voxels[idx];
        if (data.index == UINT32_MAX) {
           // auto vox = &GenVoxel(pos);
            data = GenVoxelData(pos);
           // delete vox;
            lock = false;
            SetVoxelData(data, pos);
        }
        lock = false;
        return data;
    }

    Voxel& GetVoxel(Vector3i pos) {
        auto idx = ((INT64)pos.x * ChunkSize + pos.y) * ChunkSize + pos.z;
        while (lock) Sleep(1);
        lock = true;
        Voxel* vox = nullptr;
        if (voxels[idx].index == UINT32_MAX) {
            vox = Voxel::Build(GenVoxelData(pos), pos);
            lock = false;
            SetVoxel(vox, pos);
        } else {
            vox = Voxel::Build(voxels[idx], pos);
        }
        lock = false;
        return *vox;
    }

    VoxelData GenVoxelData(Vector3i voxelPos, bool genNormals = false);

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

    Voxel& GetVoxel(int x, int y, int z) {
        return GetVoxel({ x, y, z });
    }
    Model* GetModel() {
        if (model) return model;
        return model = GenerateSmoothModel();
    }
    Model* GenerateModel();
    Model* GenerateSmoothModel();
};
