#pragma once
#include "../Scene/Component.h"

#include <GameEngine/Scene/SceneObject.h>
#include <GameEngine/Scene/Scene.h>

#include <GameEngine/Vectors/Vector3f.h>

#include "Transform.h"

namespace Game {
	class SoundSource : public Component {
	public:
		void OnInit();
		virtual ~SoundSource() = 0;
		void Update();
		void play(char* name, float volume , Vector3f position);
		void setPosition(Vector3f newPos);
		void setVolume(float volume);
	protected:
		Scene& GetScene();
		SceneRender& GetRender();
		Engine& GetEngine();
	private:
		Scene* scene;
		SoundManager* soundManager;
		std::shared_ptr<OggStream> ogg;
		Transform* transform;
	};
}