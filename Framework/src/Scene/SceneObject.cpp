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
		parent->RemoveChild(this);
		ForceDisable();
		for (auto c : components) {
			_DELETE(c);
		}
		for (auto o : children) {
			_DELETE(o);
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
		for (auto o : children) {
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
	vector<SceneObject*> SceneObject::GetChildren() { 
		std::vector<SceneObject*> vec;
		vec.reserve(children.size());
		for (auto ch : children) {
			vec.push_back(ch);
		}
		return vec; 
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

	SceneObject* SceneObject::Instantiate(SceneObject* o) {
		if (o == nullptr) THROW_NULL_PTR_EXCEPTION(o);
		o->parent = this;
		children.insert(o);
		return o;
	}

	SceneObject* SceneObject::Instantiate() {
		return Instantiate(new SceneObject(scene));
	}

	void SceneObject::RemoveChild(SceneObject* o) {
		children.erase(o);
	}

	void SceneObject::Destroy() {
		flags |= (UINT)Flags::NEED_DESTROY;
	}
}