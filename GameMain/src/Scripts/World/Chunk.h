#pragma once
#include <vector>
#include <GameEngine\Model.h>
#include <functional>

#include "Voxels/Voxel.h"
#include "WorldGenerator.h"

using namespace Game;

class HeightMap;
class World;

class Chunk {
private:
    WorldGenerator* gen;
    HeightMap* map;
    Model* model;
    World* world;

    std::vector<Voxel*> voxels = {};
    void Generate();

public:
    enum Status { Unloaded = -2, NotCreated, NotLoaded, Loading, Loaded } status;

    static const int ChunkSize;
    Vector3i position{};

    Chunk(WorldGenerator* gen, Vector3i pos, HeightMap* map, World* world) {
        voxels.resize((INT64)ChunkSize * ChunkSize * ChunkSize);
        this->gen = gen;
        this->position = pos;
        this->map = map;
        this->world = world;
    }

    Voxel& GetVoxel(int x, int y, int z) {
        auto idx = ((INT64)x * ChunkSize + y) * ChunkSize + z;
        auto vox = voxels[idx];
        if (vox == nullptr) {
            Vector3i inChunkPos{ x,y,z };
            vox = &GenVoxel(inChunkPos);
            SetVoxel(vox, inChunkPos);
        }
        return *vox;
    }

    Voxel& GenVoxel(Vector3i voxelPos, bool genNormals = false);

    void SetVoxel(Voxel* vox, Vector3i chunkPos) {
        auto x = chunkPos.x;
        auto y = chunkPos.y;
        auto z = chunkPos.z;
        voxels[(x * ChunkSize + y) * ChunkSize + z] = vox;
    }

    Voxel& GetVoxel(Vector3i pos) {
        return GetVoxel(pos.x, pos.y, pos.z);
    }
    Model* GetModel() {
        if (model == nullptr) {
            model = GenerateModel();
        }
        return model;
    }
    Model* GenerateModel();
};
