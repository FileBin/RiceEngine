#pragma once
#include <GameEngine\Core.h>
#include <GameEngine\InputManager.h>

using namespace Game;

class SamplePostProcess : public MonoScript, public IPostProcess {
public:
	ChunkGenerator* generator;
	void OnEnable() {
		GetScene().GetRender().AddPostProcessScript(this);
	}
	void PostProcess() {
		auto render = &GetRender();
		auto playerPos = render->GetActiveCamera()->position;
		auto waterDepth = generator->world->GetTransparentVoxelDepth((Vector3i)playerPos, VoxelTypeIndex::V_WATER);
		SmartPtr<Material> mat;
		if (waterDepth < -.9f) {
			mat = render->GetMaterial(L"Underwater");
			auto waterMat = render->GetMaterial(L"Water");
			waterMat->SetVar(L"mode", true);
			waterMat->UpdateBuffer();
		} else {
			mat = render->GetMaterial(L"PostMaterial");
			auto waterMat = render->GetMaterial(L"Water");
			waterMat->SetVar(L"mode", false);
			waterMat->UpdateBuffer();
		}
		render->PostProcess(mat.Get());
	}

	void OnDisable() {
		GetScene().GetRender().AddPostProcessScript(this);
	}
};