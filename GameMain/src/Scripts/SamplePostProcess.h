#pragma once
#include <GameEngine\Core.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\RenderScript.h>

using namespace Game;

class SamplePostProcess : public RenderScript {
	void Run() {
		auto mat = render->GetMaterial(L"PostMaterial");
		render->PostProcess(mat.Get());
	}
};