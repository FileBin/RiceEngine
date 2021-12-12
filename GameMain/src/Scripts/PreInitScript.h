#pragma once
#include <GameEngine\ScriptBase.h>
#include <MyRender.h>
#include <GameEngine\Log.h>

#include "MsaaTestScript.h"

using namespace Game;

class PreInitScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();
		Log::LoadLocalization(Log::Localization::LoadFromJSON(L"language/en-EN/log.json"));
		engine.SetRender(new MyRender());
		engine.SetMsaaLevel(4);

		engine.RegisterScript(new MsaaTestScript());
	}
};
