#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\MonoScript.h>
namespace Game {
	Scene::Scene() {
		engine = nullptr;
		initScript = new InitScript();
		preUpdateScript = new PreUpdateScript();
		updateScript = new UpdateScript();
		closeScript = new CloseScript();
		initScript->scene = preUpdateScript->scene = updateScript->scene = closeScript->scene = this;
		root = new SceneObject(this);
		render = new SceneRender();
	}

	Scene::~Scene() {};
	
	void Scene::AddScript(MonoScript* script) {
		root->AddComponent(script);
	}

	SceneObject& Scene::GetObjectByName(String name) { return root->GetObjectByName(name); }

	SceneObject* Scene::Instaniate() { return root->Instaniate(); }

	Engine& Scene::GetEngine() { return *engine; }

	void Scene::InitScript::Run() {
		scene->Initialize();
		scene->root->Enable();
		scene->root->Start();
	}

	void Scene::PreUpdateScript::Run() {
		scene->root->PreUpdate();
	}

	void Scene::UpdateScript::Run() {
		scene->root->Update();
	}
	void Scene::CloseScript::Run() {
		scene->Close();
	}
}