#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Component.h>

namespace Game {
	SceneObject::SceneObject(Scene* scene) { this->scene = scene; }

	SceneObject::~SceneObject() {}

	void SceneObject::Enable() {
		for (auto c : components) {
			c->Enable();
		}
		for (auto o : children) {
			o->Enable();
		}
	}
	void SceneObject::PreUpdate() {
		for (auto c : components) {
			c->PreUpdate();
		}
		for (auto o : children) {
			o->PreUpdate();
		}
	}
	void SceneObject::Update() {
		for (auto c : components) {
			c->Update();
		}
		for (auto o : children) {
			o->Update();
		}
	}
	void SceneObject::Disable() {
		for (auto c : components) {
			c->Disable();
		}
		for (auto o : children) {
			o->Disable();
		}
	}

	Scene& SceneObject::GetScene() { return *scene; }
	std::vector<SceneObject*> SceneObject::GetChildren() { return children; }

	void SceneObject::AddComponent(Component* c) {
		c->Init(this);
		components.push_back(c);
	}

	SceneObject* SceneObject::Instaniate() {
		auto o = new SceneObject(scene);
		children.push_back(o);
		return o;
	}
}