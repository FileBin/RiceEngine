#pragma once
#include <GameEngine\ScriptBase.h>
#include <GameEngine\Log.h>

//#include "InitScript.h"
#include "InitSceneScript.h"

using namespace Game;

class PreInitScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();
		Log::LoadLocalization(Log::Localization::LoadFromJSON(L"language/en-EN/log.json"));
		engine.SetMsaaLevel(4);

		engine.SetRender(new DefaultLoadingScreenRender());

		engine.RegisterScript(new InitSceneScript(), Stage::Init);
	}
};
