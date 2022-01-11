#pragma once

#include "SampleScene.h"
#include "../Scenes/PhysicsTest.h"
#include "MsaaTestScript.h"

#include <GameEngine\ScriptBase.h>
#include <GameEngine\Model.h>
#include <GameEngine\Util3D.h>

using namespace Game;

class InitSceneScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();
#ifdef _DEBUG
		engine.LoadScene(new PhysicsScene());
#else
		engine.LoadScene(new SampleScene());
#endif

		engine.RegisterScript(new MsaaTestScript());
	}
};
