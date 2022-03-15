#pragma once
#include <GameEngine\ScriptBase.h>
#include <GameEngine\Log.h>

//#include "InitScript.h"
#include "InitSceneScript.h"
#include "../Scenes/LoadingScene.h"
#include <filesystem>
using namespace Game;

class PreInitScript : public ScriptBase {
	void Run() {
		LoadFonts();
		auto& engine = GetEngine();
		engine.SetMsaaLevel(4);
		engine.SetLoadingScene(new LoadingScene());
		//engine.SetRender(new DefaultLoadingScreenRender());
		engine.SetFps(60);
		engine.RegisterScript(new InitSceneScript(), Stage::Init);
	}

	void LoadFonts() {
		using namespace std::filesystem;
		path it{ ".\\fonts\\" };
		for (path entry : directory_iterator(it)) {
			auto ex = entry.filename().extension();
			if (ex == L".ttf") {
				AddFontResourceW(entry.c_str());
			}
		}
	}
};
