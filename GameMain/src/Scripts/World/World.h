#pragma once
#include <cmath>
#include <queue>
#include <concurrent_unordered_map.h>
#include "WorldGenerator.h"
#include "Chunk.h"
#include "HeightMap.h"
#include <GameEngine\Vectors\Vector2i.h>
#include <GameEngine\Util\SmartPointer.h>
#include <GameEngine\Vectors\Hasher.h>

using namespace Game;
using namespace concurrency;

class World {
public:
#pragma region Static
	static Vector3i TransformToChunkPos(Vector3 worldPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 (num)floor(worldPosition.x / n),
			 (num)floor(worldPosition.y / n),
			 (num)floor(worldPosition.z / n),
		};
	}

	static Vector2i ToTerrainPos(Vector3i chunkPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 chunkPosition.x,
			 chunkPosition.z,
		};
	}

	static Vector3 TransformToWorldPos(Vector3i chunkPosition) {
		auto n = Chunk::ChunkSize;
		return {
			 (dbl)chunkPosition.x * n,
			 (dbl)chunkPosition.y * n,
			 (dbl)chunkPosition.z * n,
		};
	}
#pragma endregion
#pragma region Fields
private:
	concurrent_unordered_map<Vector3i, SmartPtr<Chunk>> chunkMap{};
	concurrent_unordered_map<Vector2i, SmartPtr<HeightMap>> heightMaps{};
	bool lock = false;
	WorldGenerator& generator;
#pragma endregion

#pragma region publicFunctions
public:
	void Wait() { while (lock) Sleep(1); }
	void Lock() { lock = true; }
	void Unlock() { lock = false; }

	static void Register(Engine& en, SceneRender& ren);

	World(WorldGenerator* gen, SceneRender* ren) : generator(*gen) { Voxel::Register(*ren); }

	void UnloadChunks(std::function<bool(Vector3i)> predicate) {
		std::queue<Vector3i> keys;
		for (auto it = chunkMap.begin(); it != chunkMap.end(); it++) {
			keys.push(it->first);
		}
		while (!keys.empty()) {
			auto k = keys.front();
			keys.pop();
			if (!predicate(k)) {
				auto it = chunkMap.find(k);
				chunkMap.unsafe_erase(it);
				it->second.Release();
				auto hIt = heightMaps.find(ToTerrainPos(k));
				if (hIt == heightMaps.end()) continue;
				heightMaps.unsafe_erase(hIt);
				hIt->second.Release();
			}
		}
	}

	void UnloadChunk(Vector3i chunkPos) {
		auto it = chunkMap.find(chunkPos);
		//if (it != chunkMap.end()) {
			auto chunk = it->second;
			SaveChunk(*chunk);
			Vector2i hmPos = { chunkPos.x, chunkPos.z };
			chunkMap.unsafe_erase(chunkPos);
			chunk.Release();
			auto hIt = heightMaps.find(hmPos);
			if (hIt != heightMaps.end()) {
				auto hm = hIt->second;
				hm->Decrement();
				SaveHeightMap(*hm);
				heightMaps.unsafe_erase(hmPos);
				hm.Release();
			}
		//}
	}

	void SaveChunk(Chunk& chunk) {}

	void SaveHeightMap(HeightMap& hm) {}

	std::vector<SmartPtr<Chunk>> GetChunks(Vector2i hmPos) {
		std::vector<SmartPtr<Chunk>> chunks{};
		for (auto ch = chunkMap.begin(); ch != chunkMap.end(); ch++) {
			if (Vector2i(ch->second->position.x, ch->second->position.z) == hmPos) {
				chunks.push_back({ ch->second });
			}
		}
		return chunks;
	}

	float GetAltitude(Vector3 pos) {
		auto chunkPos = TransformToChunkPos(pos);
		auto hm = GetHeightMap(chunkPos);
		Vector3i iPos = pos;
		auto localPos = iPos - chunkPos * Chunk::ChunkSize;
		double height = hm->Get(localPos.x, localPos.z);
		return pos.y - height;
	}

	int GetChunkAltitude(Vector3i chunkPos) {
		auto hm = GetHeightMap(chunkPos);
		auto hmax = (chunkPos.y + 1.) * Chunk::ChunkSize;
		auto hmin = (chunkPos.y) * Chunk::ChunkSize;
		if (hmin > hm->MaxHeight()) {
			return chunkPos.y - (int)floor(hm->MaxHeight() / Chunk::ChunkSize);
		} else if (hmax < hm->MinHeight()) {
			return chunkPos.y - (int)ceil(hm->MinHeight() / Chunk::ChunkSize);
		}
		return 0;
	}

	Chunk::Status GetChunkStatus(Vector3i chunkPos) {
		//Wait();
		//Lock();
		auto it = chunkMap.find(chunkPos);
		if (it != chunkMap.end()) {
			Unlock();
			return it->second->status;
		}
		//Unlock();
		return Chunk::NotCreated;
	}

	void SetChunkStatus(Vector3i chunkPos, Chunk::Status status) {
		auto it = chunkMap.find(chunkPos);
		if (it != chunkMap.end()) {
			//Wait();
			//Lock();
			it->second->status = status;
			//Unlock();
		}
	}

	SmartPtr<Chunk> GetChunk(Vector3i chunkPos) {
		auto it = chunkMap.find(chunkPos);
		if (it != chunkMap.end()) {
			return it->second;
		} else {
			auto ch = GenerateChunk(chunkPos);
			chunkMap.insert(chunkMap.end(), { chunkPos, ch });
			return ch;
		}
	}

	float GetTransparentVoxelDepth(Vector3i worldPos, size_t idx) {
		auto chunk = TransformToChunkPos(worldPos);
		worldPos = worldPos - chunk * Chunk::ChunkSize;
		return GetChunk(chunk)->GetTranspDepth(worldPos, idx);
	}

	Voxel& GetVoxel(Vector3i voxelPos) {
		Vector3i& vox = voxelPos;
		auto chunk = TransformToChunkPos(voxelPos);
		vox = vox - chunk * Chunk::ChunkSize;
		return GetChunk(chunk)->GetVoxel(vox);
	}

	bool IsVoxelVoid(Vector3i voxelPos) {
		Vector3i& vox = voxelPos;
		auto chunk = TransformToChunkPos(voxelPos);
		vox = vox - chunk * Chunk::ChunkSize;
		return GetChunk(chunk)->IsVoxelVoid(vox);
	}

	VoxelData GetVoxelData(Vector3i voxelPos) {
		Vector3i& vox = voxelPos;
		auto chunk = TransformToChunkPos(voxelPos);
		vox = vox - chunk * Chunk::ChunkSize;
		return GetChunk(chunk)->GetData(vox);
	}

	SmartPtr<HeightMap> GetHeightMap(Vector3i chunkPos) {
		Vector2i pos{ chunkPos.x, chunkPos.z };
		auto it = heightMaps.find(pos);
		if (it != heightMaps.end()) {
			return it->second;
		} else {
			auto hm = CreateHeightMap(pos);
			heightMaps.insert(heightMaps.end(), { pos, hm });
			return hm;
		}
	}
#pragma endregion
#pragma region PrivateFunctions
private:
	SmartPtr<Chunk> GenerateChunk(Vector3i chunkPos) {
		auto hm = GetHeightMap(chunkPos);
		auto chunk = new Chunk(&generator, chunkPos, hm, this);
		hm->Increment();
		return chunk;
	}

	SmartPtr<HeightMap> CreateHeightMap(Vector2i pos) {
		auto hm = new HeightMap(generator, pos);
		return hm;
	}
#pragma endregion
};
