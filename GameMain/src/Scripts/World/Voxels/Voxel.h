#pragma once

#include <GameEngine\Vectors\Vector3i.h>
#include "VoxelData.h"
#include <concurrent_unordered_map.h>

using concurrency::concurrent_unordered_map;

struct Voxel {
public:
	Vector3i position;
	bool isVoid;
	virtual VoxelData GetData() = 0;
protected:
	static concurrent_unordered_map <uint, std::function<Voxel*(VoxelData&, Vector3i&)>> builders;
	static concurrent_unordered_map <uint, bool> voidMap;
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

	static void Register();
};
