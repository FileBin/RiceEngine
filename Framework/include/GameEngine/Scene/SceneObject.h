#pragma once

#include "../Util.h"
#include <vector>

namespace Game {
	class Scene;
	class Component;

	class SceneObject  {
	public:
		SceneObject (Scene* scene);
		~SceneObject ();

		void Enable();
		void PreUpdate();
		void Update();
		void Disable(); 

		std::vector<SceneObject*> GetChildren();

		void AddComponent(Component* component);
		SceneObject* Instaniate();

		void SetName(String name) { this->name = name; }
		Scene& GetScene();

	private:
		bool init = false;

		Scene* scene = nullptr;
		String name = L"";
		std::vector<Component*> components = {};
		std::vector<SceneObject*> children = {};
	};
}

