#pragma once
#include "../Scene/Component.h"

namespace Game {
	class Engine;
	class SceneRender;
	class Scene;

	class MonoScript : public Component {
		void OnInit();
		virtual void Initialize() {}
	protected:
		Scene& GetScene();
		SceneRender& GetRender();
		Engine& GetEngine();
	private:
		Scene* scene;
	};
}