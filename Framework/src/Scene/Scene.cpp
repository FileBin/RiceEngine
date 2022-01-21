#include "pch.h"
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\MonoScript.h>
namespace Game {
	Scene::Scene() {
		engine = nullptr;
		soundManager = nullptr;
		root = new SceneObject(this);
		render = new SceneRender();
	}

	Scene::~Scene() { soundManager->~SoundManager(); };
	
	void Scene::AddScript(MonoScript* script) {
		root->AddComponent(script);
	}


	SceneObject& Scene::GetObjectByName(String name) { return root->GetObjectByName(name); }

	SceneObject* Scene::Instaniate() { return root->Instaniate(); }

	SoundManager& Scene::GetSoundManager() const { return *soundManager; }

	const SmartPtr<Physics::PhysicsEngine> Scene::GetPhysEngine() const {
		return physicsEngine;
	}

	Engine& Scene::GetEngine() const { return *engine; }
}