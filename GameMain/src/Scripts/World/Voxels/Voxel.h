#pragma once

#include <GameEngine\Vectors\Vector3i.h>
#include "VoxelData.h"
#include <concurrent_unordered_map.h>

#include <GameEngine\Material.h>

using concurrency::concurrent_unordered_map;

namespace Game {
	class Engine;
	class SceneRender;
}

struct Voxel {
public:
	Vector3i position;
	bool isVoid;
	virtual VoxelData GetData() = 0;
protected:
	static concurrent_unordered_map <uint, std::function<Voxel*(VoxelData&, Vector3i&)>> builders;
	static concurrent_unordered_map <uint, bool> voidMap;
	static concurrent_unordered_map <uint, Game::Material*> materialMap;
public:
	static Voxel* Build(VoxelData data, Vector3i position) {
		auto it = builders.find(data.index);
		if (it != builders.end()) {
			return it->second(data, position);
		}
		throw std::exception("Voxel index invalid!");
	}

	static bool IsVoid(uint index) {
		auto it = voidMap.find(index);
		if (it != voidMap.end()) {
			return it->second;
		}
		throw std::exception("Voxel index invalid!");
	}

	static size_t GetMaterialCount() { return materialMap.size(); }
	static Game::Material& GetMaterialAt(size_t idx) {
		auto it = materialMap.find(idx);
		if (it != materialMap.end()) {
			return *it->second;
		}
		throw std::exception("Material index invalid!");
	}

	static void Register(Game::Engine& en, Game::SceneRender& ren);
};
