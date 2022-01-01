#pragma once

#include "../Util.h"
#include <vector>
#include <concurrent_vector.h>
#include "Component.h"

namespace Game {
	using namespace concurrency;

	class Scene;

	class SceneObject  {
	public:
		SceneObject (Scene* scene);
		~SceneObject();

		void Enable();
		void Start();
		void PreUpdate();
		void Update();
		void Disable(); 

		bool isEnabled() { return enabled; }

		concurrent_vector<SceneObject*> GetChildren();

		template<typename T>
		std::vector<T*> GetComponents() {
			std::vector<T*> vec = {};
			for (auto c : components) {
				auto o = dynamic_cast<T*>(c);
				if (o != nullptr) { 
					vec.push_back(o);
				}
			}
			return vec;
		}

		SceneObject& GetObjectByName(String name);
		bool TryGetObjectByName(String& name, SceneObject* &object);

		void AddComponent(Component* component);
		SceneObject* Instaniate();

		void SetName(String name) { this->name = name; }
		Scene& GetScene();

	private:
		bool init = false;
		bool enabled = false;
		Scene* scene = nullptr;
		String name = L"";
		concurrent_vector<Component*> components{};
		concurrent_vector<SceneObject*> children{};
	};
}

