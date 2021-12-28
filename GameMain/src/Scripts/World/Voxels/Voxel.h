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

enum VoxelTypeIndex { V_VOID, V_GRASS, V_DARK_GRASS, V_STONE, V_DARK_STONE, V_DIRT, V_SNOW, V_SAND, V_WATER };

struct Voxel {
public:
	Vector3i position;
	bool isVoid;
	bool isTransparent;
	virtual VoxelData GetData() = 0;
protected:
	static concurrent_unordered_map <uint, std::function<Voxel*(VoxelData&, Vector3i&)>> builders;
	static concurrent_unordered_map <uint, bool> voidMap;
	static concurrent_unordered_map <uint, bool> transparentMap;
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

	static bool IsTransparent(uint index) {
		auto it = transparentMap.find(index);
		if (it != transparentMap.end()) {
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

	template<typename V>
	static void registerVoxel(Game::Engine& en, Game::SceneRender& ren) {
		builders.insert({ V::GetIdx(), V::Build });
		voidMap.insert({ V::GetIdx(), V::IsVoid() });
		transparentMap.insert({ V::GetIdx(), V::IsTransparent() });
		materialMap.insert({ V::GetIdx(), &V::CreateMaterial(en,ren) });
	}

	static void Register(Game::Engine& en, Game::SceneRender& ren);
};
