#include "pch.h"
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Component.h>
#include <GameEngine\stdafx.h>
#include <GameEngine\Util\exceptions.h>
#include <GameEngine\Scene\Scene.h>

#define suicide delete this

namespace Game {
	SceneObject::SceneObject(Scene* scene) { this->scene = scene; }

	SceneObject::~SceneObject() {
		if (parent)
			parent->RemoveChild(this);
		ForceDisable();
		for (auto c : components) {
			c.Release();
		}
		for (auto o : children) {
			o->parent = nullptr;
			o.Release();
		}
	}

	void SceneObject::Enable() {
		flags |= (UINT)Flags::NEED_ENABLE;
	}

	void SceneObject::Disable() {
		flags |= (UINT)Flags::NEED_DISABLE;
	}

	void SceneObject::ForceEnable() {
		if (enabled) return;
		enabled = true;
		for (auto c : components) {
			c->Enable();
		}
		for (auto c : components) {
			c->Start();
		}
		auto n = children.size();
		for (size_t i = 0; i < n; i++) {
			auto o = children[i];
			if (o->active)
				o->ForceEnable();
		}
	}

	void SceneObject::PreUpdate() {
		auto en = flags & (UINT)Flags::NEED_ENABLE;
		auto dis = flags & (UINT)Flags::NEED_DISABLE;
		auto destroy = flags & (UINT)Flags::NEED_DESTROY;
		if (destroy) {
			suicide;
			return;
		}
		if (en ^ dis) {
			if (en) {
				ForceEnable();
			} else if (dis) {
				ForceDisable();
			}
		}
		flags = 0;

		for (auto c : components) {
			c->PreUpdate();
		}
		for (auto o : children) {
			o->PreUpdate();
		}
	}
	void SceneObject::Update() {
		if (!active) return;
		if (!enabled) return;
		for (auto c : components) {
			c->Update();
		}
		for (auto o : children) {
			o->Update();
		}
	}
	void SceneObject::ForceDisable() {
		if (!enabled) return;
		enabled = false;
		for (auto c : components) {
			c->Disable();
		}
		for (auto o : children) {
			o->ForceDisable();
		}
	}

	Scene& SceneObject::GetScene() { return *scene; }
	vector<SmartPtr<SceneObject>> SceneObject::GetChildren() {
		return children; 
	}

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
				object = o.Get();
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

	void SceneObject::AddComponent(SmartPtr<Component> c) {
		c->PreInit(this);
		components.push_back(c);
	}

	void SceneObject::RemoveComponent(SmartPtr<Component> c) {
		for (auto it = components.begin(); it != components.end();it++) {
			if (*it == c) {
				c->Disable();
				components.erase(it);
				return;
			}
		}
	}

	SceneObject* SceneObject::Instantiate(SceneObject* o) {
		if (o == nullptr) THROW_NULL_PTR_EXCEPTION(o);
		o->parent = this;
		children.push_back(o);
		return o;
	}

	SceneObject* SceneObject::Instantiate() {
		return Instantiate(new SceneObject(scene));
	}

	void SceneObject::RemoveChild(SmartPtr<SceneObject> o) {
		for (auto it = children.begin(); it != children.end(); it++) {
			if (*it == o) {
				o->Disable();
				children.erase(it);
				return;
			}
		}
	}

	void SceneObject::Destroy() {
		flags |= (UINT)Flags::NEED_DESTROY;
	}
}