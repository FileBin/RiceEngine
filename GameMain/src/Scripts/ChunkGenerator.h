#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <Scripts\World\Chunk.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>

using namespace Game;

class ChunkGenerator : public MonoScript {
	Chunk chunk;
	void Start() {
		auto& scene = GetScene();
		auto& modelRender = *scene.GetObjectByName(L"Chunk").GetComponents<ModelRender>()[0];
		auto A = Chunk::ChunkSize;
		for (int i = 0; i < A; i++) {
			for (int j = 0; j < A; j++) {
				for (int k = 0; k < A; k++) {
					chunk.SetVoxel({ {i,j,k}, rand() % 2 == 0 });
				}
			}
		}
		auto model = chunk.GenerateModel();
		modelRender.SetModel(model);
	}
};