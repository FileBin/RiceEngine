#pragma once
#include <GameEngine\Core.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\RenderScript.h>

using namespace Game;

class SamplePostProcess : public RenderScript {
public:
	ChunkGenerator* generator;
	void Run() {
		auto playerPos = render->GetActiveCamera()->position;
		auto waterDepth = generator->world->GetTransparentVoxelDepth((Vector3i)playerPos, VoxelTypeIndex::V_WATER);
		SmartPtr<Material> mat;
		if (waterDepth < -.9f) {
			mat = render->GetMaterial(L"Underwater");
		} else {
			mat = render->GetMaterial(L"PostMaterial");
		}
		render->PostProcess(mat.Get());
	}
};