#pragma once

#include "SampleScene.h"
#include "MsaaTestScript.h"

#include <GameEngine\ScriptBase.h>
#include <GameEngine\Model.h>
#include <GameEngine\Util3D.h>

using namespace Game;

class InitSceneScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();

		engine.LoadScene(new SampleScene());

		engine.RegisterScript(new MsaaTestScript());
	}
};
