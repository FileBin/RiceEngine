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
		void play(FrequencyFunc f, dbl beginning, dbl end, float volume);
		void setPosition(Vector3f newPos);
		void setVolume(float volume);
		void addEffect(SoundEffect *soundEffect);
		SoundEffect* addEffect(ALenum effect);
		void setLastAddedEffectPropertyf(ALenum property, ALfloat value);
		void setLastAddedEffectPropertyi(ALenum property, ALint value);
		void setLastAddedEffectPropertyfv(ALenum property, const ALfloat* values);
		void setLastAddedEffectPropertyiv(ALenum property, const ALint* values);
		void setLastAddedEffectEfxReverbPreset(EFXEAXREVERBPROPERTIES preset);
	protected:
		Scene& GetScene();
		SceneRender& GetRender();
		Engine& GetEngine();
	private:
		Scene* scene;
		SoundManager* soundManager;
		SmartPtr<SoundStream> ogg;
		std::vector<SoundEffect*> effects;
		Transform* transform;
	};
}