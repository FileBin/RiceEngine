#pragma once

#include "../Util.h"
#include <vector>
#include <concurrent_vector.h>
#include <unordered_set>
#include "Component.h"

namespace Game {
	using namespace concurrency;

	class Scene;

	class SceneObject  {
	public:
		enum class Flags {
			NONE = 0x0,
			NEED_ENABLE = 0x1,
			NEED_DISABLE = 0x2,
			NEED_DESTROY = 0x40000000,
		};

		SceneObject (Scene* scene);
		~SceneObject();

		bool isEnabled() { return enabled; }

		_STD vector<SmartPtr<SceneObject>> GetChildren();

		template<typename T>
		_STD vector<T*> GetComponents() {
			_STD vector<T*> vec = {};
			for (auto c : components) {
				auto o = dynamic_cast<T*>(c.Get());
				if (o != nullptr) { 
					vec.push_back(o);
				}
			}
			return vec;
		}

		SceneObject& GetObjectByName(String name);
		SceneObject& GetParent() { return *parent; }
		bool TryGetObjectByName(String& name, SceneObject* &object);

		void AddComponent(SmartPtr<Component> component);
		void RemoveComponent(SmartPtr<Component> component);

		SceneObject* Instantiate(SceneObject* orig);
		SceneObject* Instantiate();
		void RemoveChild(SmartPtr<SceneObject> whatObj);

		void Destroy();

		void SetName(String name) { this->name = name; }
		Scene& GetScene();

		void SetActive(bool _active) {
			active = _active; 
			if (active) {
				Enable();
			} else {
				Disable();
			}
		}

		void SetActiveImmediate(bool _active) {
			active = _active;
			if (active) {
				ForceEnable();
			} else {
				ForceDisable();
			}
		}
		bool GetActive() { return active; }

	private:

		friend class Scene;

		void Enable();
		void ForceEnable();
		void PreUpdate();
		void Update();
		void Disable();
		void ForceDisable();

		UINT flags = 0;

		bool active = true;
		bool enabled = false;
		Scene* scene = nullptr;
		SceneObject* parent = nullptr;
		String name = L"";
		_STD vector<SmartPtr<Component>> components{};
		_STD vector<SmartPtr<SceneObject>> children{};
	};
}

