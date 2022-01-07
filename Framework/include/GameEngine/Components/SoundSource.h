#pragma once
#include "../Scene/Component.h"

#include <GameEngine/Scene/SceneObject.h>
#include <GameEngine/Scene/Scene.h>

#include <GameEngine/Vectors/Vector3f.h>

#include <GameEngine/SoundEffect.h>

#include "Transform.h"

namespace Game {

	typedef char (*FrequencyFunc)(double);

	class SoundSource : public Component {
	public:
		void OnInit();
		virtual ~SoundSource() = 0;
		void Update();
		void play(char* name, float volume);
		void play(FrequencyFunc f, double beginning, double end, float volume);
		void setPosition(Vector3f newPos);
		void setVolume(float volume);
		void addEffect(SoundEffect *soundEffect);
	protected:
		Scene& GetScene();
		SceneRender& GetRender();
		Engine& GetEngine();
	private:
		Scene* scene;
		SoundManager* soundManager;
		std::shared_ptr<SoundStream> ogg;
		std::vector<SoundEffect*> effects;
		Transform* transform;
	};
}