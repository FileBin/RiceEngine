#pragma once
#include <cmath>
#include <concurrent_unordered_map.h>
#include "WorldGenerator.h"
#include "Chunk.h"
#include "HeightMap.h"
#include <GameEngine\Vectors\Vector2i.h>

using namespace Game;
using namespace concurrency;

class World {
public:
#pragma region Static
	static Vector3i TransformToChunkPos(Vector3 worldPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 (int)floor(worldPosition.x / n),
			 (int)floor(worldPosition.y / n),
			 (int)floor(worldPosition.z / n),
		};
	}

	static Vector2i ToTerrainPos(Vector3i chunkPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 chunkPosition.x,
			 chunkPosition.y,
		};
	}

	static Vector3 TransformToWorldPos(Vector3i chunkPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 chunkPosition.x * n,
			 chunkPosition.y * n,
			 chunkPosition.z * n,
		};
	}
#pragma endregion
#pragma region Fields
private:
	concurrent_unordered_map<Vector3i, Chunk> chunkMap{};
	concurrent_unordered_map<Vector2i, HeightMap> heightMaps{};

	WorldGenerator& generator;
#pragma endregion

#pragma region publicFunctions
public:
	World(WorldGenerator* gen) : generator(*gen) {}

	void UnloadChunk(Vector3i chunkPos) {
		Chunk& chunk = chunkMap[chunkPos];
		if (&chunk != nullptr) {
			SaveChunk(chunk);
			auto hmPos = ToTerrainPos(chunk.position);
			auto n = GetChunks(hmPos).size();
			chunkMap.unsafe_erase(chunkPos);

			if (n == 0) {
				HeightMap& hm = heightMaps[hmPos];
				if (&hm != nullptr) {
					SaveHeightMap(hm);
					heightMaps.unsafe_erase(hmPos);
				}
			}
		}
	}

	void SaveChunk(Chunk chunk) {}

	void SaveHeightMap(HeightMap hm) {}

	std::vector<Chunk> GetChunks(Vector2i hmPos) {
		std::vector<Chunk> chunks{};
		for (auto ch = chunkMap.begin(); ch != chunkMap.end(); ch++) {
			if (ToTerrainPos(ch->second.position) == hmPos) {
				chunks.push_back(ch->second);
			}
		}
		return chunks;
	}

	float GetAltitude(Vector3 pos) {
		auto chunkPos = TransformToChunkPos(pos);
		auto hm = GetHeightMap(chunkPos);
		Vector3i iPos = pos;
		auto localPos = iPos - chunkPos * Chunk::ChunkSize;
		double height = hm.Get(localPos.x, localPos.z);
		return pos.y - height;
	}

	int GetChunkAltitude(Vector3i chunkPos) {
		auto hm = GetHeightMap(chunkPos);
		auto hmax = (chunkPos.y + 1.) * Chunk::ChunkSize;
		auto hmin = (chunkPos.y) * Chunk::ChunkSize;
		if (hmin > hm.MaxHeight()) {
			return chunkPos.y - (int)floor(hm.MaxHeight() / Chunk::ChunkSize);
		} else if (hmax < hm.MinHeight()) {
			return chunkPos.y - (int)ceil(hm.MinHeight() / Chunk::ChunkSize);
		}
		return 0;
	}

	Chunk::Status GetChunkStatus(Vector3i chunkPos) {
		auto it = chunkMap.find(chunkPos);
		if (it != chunkMap.end()) {
			return it->second.status;
		}
		return Chunk::NotCreated;
	}

	void SetChunkStatus(Vector3i chunkPos, Chunk::Status status) {
		auto chunk = GetChunk(chunkPos);
		chunk.status = status;
	}

	Chunk& GetChunk(Vector3i chunkPos) {
		auto it = chunkMap.find(chunkPos);
		if (it != chunkMap.end()) {
			return it->second;
		} else {
			auto& ch = GenerateChunk(chunkPos);
			chunkMap.insert(chunkMap.end(), ch);
			return ch;
		}
	}

	Voxel& GetVoxel(Vector3i voxelPos) {
		Vector3 vox = voxelPos;
		auto pos = (vox / Chunk::ChunkSize);
		Vector3 chunk{ floor(pos.x), floor(pos.y), floor(pos.z)};
		vox = vox - chunk * Chunk::ChunkSize;
		return GetChunk(chunk).GetVoxel(vox);
	}
#pragma endregion
#pragma region PrivateFunctions
private:
	Chunk& GenerateChunk(Vector3i chunkPos) {
		auto& hm = GetHeightMap(chunkPos);
		auto chunk = new Chunk(&generator, chunkPos, &hm);
		return *chunk;
	}

	HeightMap& GetHeightMap(Vector3i chunkPos) {
		Vector2i pos{ chunkPos.x, chunkPos.z };
		auto it = heightMaps.find(pos);
		if (it != heightMaps.end()) {
			return it->second;
		} else {
			auto& ch = CreateHeightMap(pos);
			chunkMap.insert(chunkMap.end(), ch);
			return ch;
		}
	}

	HeightMap& CreateHeightMap(Vector2i pos) {
		auto hm = new HeightMap(generator, pos);
		return *hm;
	}
#pragma endregion
};
