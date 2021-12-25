#pragma once
#include "../Scene/Component.h"

namespace Game {
	class Engine;
	class SceneRender;
	class Scene;

	class MonoScript : public Component {
	public:
		void OnInit();
		virtual void Initialize() {}
		virtual ~MonoScript() = 0;
	protected:
		Scene& GetScene();
		SceneRender& GetRender();
		Engine& GetEngine();
	private:
		Scene* scene;
	};
}