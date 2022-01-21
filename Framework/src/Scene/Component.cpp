#include "pch.h"
#include <GameEngine\Scene\Component.h>
#include <GameEngine\Scene\SceneObject.h>
namespace Game {
	void Component::PreInit(SceneObject* parent) {
		obj = parent; 
		OnInit();
	}
	Component::~Component() {};
	SceneObject& Component::GetSceneObject() { return *obj; }
}