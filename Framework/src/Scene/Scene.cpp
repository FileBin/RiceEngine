#include <GameEngine\Scene\Scene.h>

namespace Game {
	Scene::Scene() {
		initScript = new InitScript();
		preUpdateScript = new PreUpdateScript();
		updateScript = new UpdateScript();
		initScript->scene = preUpdateScript->scene = updateScript->scene = this;
		root = new SceneObject(this);
		render = new SceneRender();
	}

	SceneObject* Scene::Instaniate() { return root->Instaniate(); }

	Engine& Scene::GetEngine() { return *engine; }

	void Scene::InitScript::Run() {
		scene->Initialize();
		for (auto obj : scene->root->GetChildren()) {
			obj->Enable();
		}
	}

	void Scene::PreUpdateScript::Run() {
		for (auto obj : scene->root->GetChildren()) {
			obj->PreUpdate();
		}
	}

	void Scene::UpdateScript::Run() {
		for (auto obj : scene->root->GetChildren()) {
			obj->Update();
		}
	}
}