#include "pch.h"
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Component.h>
#include <GameEngine\stdafx.h>
#include <GameEngine\Util\exceptions.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	SceneObject::SceneObject(Scene* scene) { this->scene = scene; }

	SceneObject::~SceneObject() {
		for (auto c : components) delete c;
		for (auto o : children) delete o;
	}

	void SceneObject::Enable() {
		if (enabled) return;
		enabled = true;
		for (auto c : components) {
			c->Enable();
		}
		for (auto o : children) {
			o->Enable();
		}
	}

	void SceneObject::Start() {
		for (auto c : components) {
			c->Start();
		}
		for (auto o : children) {
			o->Start();
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
		if (!enabled) return;
		for (auto c : components) {
			c->Update();
		}
		for (auto o : children) {
			o->Update();
		}
	}
	void SceneObject::Disable() {
		if (!enabled) return;
		enabled = false;
		for (auto c : components) {
			c->Disable();
		}
		for (auto o : children) {
			o->Disable();
		}
	}

	Scene& SceneObject::GetScene() { return *scene; }
	concurrent_vector<SceneObject*> SceneObject::GetChildren() { return children; }

	SceneObject& SceneObject::GetObjectByName(String name) {
		SceneObject* o = nullptr;
		if (TryGetObjectByName(name, o)) {
			return *o;
		}
		THROW_EXCEPTION("Object not found");
	}

	bool SceneObject::TryGetObjectByName(String& name, SceneObject* &object) {
		for (auto o : children) {
			if (o->name == name) {
				object = o;
				return true;
			}
		}
		for (auto ch : children) {
			if (ch->TryGetObjectByName(name, object)) {
				return true;
			}
		}
		return false;
	}

	void SceneObject::AddComponent(Component* c) {
		c->PreInit(this);
		components.insert(c);
	}

	void SceneObject::RemoveComponent(Component* c) {
		c->Disable();
		components.erase(c);
	}

	SceneObject* SceneObject::Instaniate() {
		auto o = new SceneObject(scene);
		children.push_back(o);
		return o;
	}
}