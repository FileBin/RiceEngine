#pragma once

#include "../Scenes/SampleScene.h"
#include "../Scenes/MainMenuScene.h"
#include "../Scenes/PhysicsTest.h"
#include "MsaaTestScript.h"

#include <GameEngine\ScriptBase.h>
#include <GameEngine\Model.h>
#include <GameEngine\Util3D.h>
#include <GameEngine\DefaultLoadingScreenRender.h>

using namespace Game;

class InitSceneScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();
		auto device = engine.GetDevice();
		//device->CreateFontCollection(L"Hyperblox", L"./fonts/Hypeblox-L3YGZ.ttf");
//#ifdef _DEBUG
	//	engine.LoadScene(new PhysicsScene());
//#else
		//engine.LoadScene(new SampleScene());
		engine.LoadScene(new MainMenuScene());
//#endif

		engine.RegisterScript(new MsaaTestScript());
	}
};
