#pragma once

#include "Component.h"

namespace Game {
	class Scene;

	class SceneScript: public Component {
	public:
		SceneScript(Scene* scene);
		~SceneScript();

		virtual void Start() = 0;

		virtual void PreUpdate() = 0;
		virtual void Update() = 0;
	protected:
		Scene& GetScene();
	private:
		Scene* scene = nullptr;
	};

	SceneScript::SceneScript(Scene* scene) {
		this->scene = scene;
	}

	SceneScript::~SceneScript() {}

	Scene& SceneScript::GetScene() {
		return *scene;
	}
}
