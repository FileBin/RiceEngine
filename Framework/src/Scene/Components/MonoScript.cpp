#include "pch.h"
#include <GameEngine\Components\MonoScript.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	void MonoScript::OnInit() {
		scene = &GetSceneObject().GetScene();
		Initialize();
	}
	MonoScript::~MonoScript() {};
	Scene& MonoScript::GetScene() { return *scene; }
	SceneRender& MonoScript::GetRender() { return GetScene().GetRender(); }
	Engine& MonoScript::GetEngine() { return GetScene().GetEngine(); }
}