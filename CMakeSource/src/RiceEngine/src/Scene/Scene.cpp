#include "pch.h"
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\MonoScript.h>
namespace Game {
	Scene::Scene() {
		engine = nullptr;
		soundManager = nullptr;
		root = new SceneObject(this);
		render = new SceneRender(this);
	}
	
	void Scene::AddScript(MonoScript* script) {
		root->AddComponent(script);
	}


	SceneObject& Scene::GetObjectByName(String name) { return root->GetObjectByName(name); }

	SceneObject* Scene::Instaniate(SceneObject* orig) { return root->Instantiate(orig); }
	SceneObject* Scene::Instaniate() { return root->Instantiate(); }

	SoundManager& Scene::GetSoundManager() const { return *soundManager; }

	const SmartPtr<Physics::PhysicsEngine> Scene::GetPhysEngine() const {
		return physicsEngine;
	}

	Engine& Scene::GetEngine() const { return *engine; }
}