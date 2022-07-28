#pragma once

#include <GameEngine\Vectors\Vector3i.h>
#include <GameEngine\Vectors\Vector4f.h>
#include "VoxelData.h"
#include <concurrent_unordered_map.h>

#include <GameEngine\Material.h>

#include "GameEngine\Util\exceptions.h"

using concurrency::concurrent_unordered_map;

namespace Game {
	class Engine;
	class SceneRender;
}

enum VoxelTypeIndex { V_VOID = 0, V_GRASS = 1, V_DARK_GRASS = 2, V_STONE = 3, V_DARK_STONE = 4, V_DIRT = 5, V_SNOW = 6, V_SAND = 7, V_WATER = 8, COUNT };

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
	static concurrent_unordered_map <uint, SmartPtr<Game::Material>> materialMap;
public:
	static Voxel* Build(VoxelData data, Vector3i position) {
		auto it = builders.find(data.index);
		if (it != builders.end()) {
			return it->second(data, position);
		}
		THROW_INVALID_ARG_EXCEPTION("data.index");
	}

	static bool IsVoid(uint index) {
		auto it = voidMap.find(index);
		if (it != voidMap.end()) {
			return it->second;
		}
		THROW_INVALID_ARG_EXCEPTION("index");
	}

	static bool IsTransparent(uint index) {
		auto it = transparentMap.find(index);
		if (it != transparentMap.end()) {
			return it->second;
		}
		THROW_INVALID_ARG_EXCEPTION("index");
	}

	static size_t GetMaterialCount() { return materialMap.size(); }
	static SmartPtr<Game::Material> GetMaterialAt(size_t idx) {
		auto it = materialMap.find(idx);
		if (it != materialMap.end()) {
			return it->second;
		}
		THROW_INVALID_ARG_EXCEPTION("idx");
	}

	template<typename V>
	static void registerVoxel(Game::SceneRender& ren) {
		builders.insert({ V::GetIdx(), V::Build });
		voidMap.insert({ V::GetIdx(), V::IsVoid() });
		transparentMap.insert({ V::GetIdx(), V::IsTransparent() });
		materialMap.insert({ V::GetIdx(), V::CreateMaterial(ren) });
	}

	static void Register(Game::SceneRender& ren);
};
