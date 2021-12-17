#pragma once

#include "PostInitScript.h"

#include "SampleScene.h"

#include <GameEngine\ScriptBase.h>
#include <GameEngine\Model.h>
#include <GameEngine\Util3D.h>

using namespace Game;

class InitScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();

		engine.LoadScene(new SampleScene());

		engine.RegisterScript(new MsaaTestScript());
	}
};
