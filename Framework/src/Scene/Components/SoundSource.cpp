#include "pch.h"
#include <GameEngine/Components/SoundSource.h>

namespace Game {
	void SoundSource::OnInit() {
		scene = &GetSceneObject().GetScene();
		soundManager = &scene->GetSoundManager();
		ogg = nullptr;
		transform = GetSceneObject().GetComponents<Transform>()[0];
	}
	SoundSource::~SoundSource() {
		for (int i = effects.size() - 1; i >= 0; i--) {
			effects.at(i)->~SoundEffect();
		}
		effects.clear();
	};
	Scene& SoundSource::GetScene() { return *scene; }
	SceneRender& SoundSource::GetRender() { return GetScene().GetRender(); }
	Engine& SoundSource::GetEngine() { return GetScene().GetEngine(); }
	void SoundSource::Update() {
		setPosition(transform->position);
	}
	void SoundSource::play(char* name, float volume) {
		ogg = soundManager->play_sound(name, volume, transform->position).lock();
		ogg->applyEffectChain(&effects);
	}
	void SoundSource::play(FrequencyFunc f, double beginning, double end, float volume) {
		ogg = soundManager->play_raw(f, beginning, end, volume, transform->position).lock();
		ogg->applyEffectChain(&effects);
	}
	void SoundSource::setPosition(Vector3f position) {
		if (ogg != nullptr) {
			if (ogg->playing()) {
				ogg->setPosition(position);
			}
		}
	}
	void SoundSource::setVolume(float volume) {
		if (ogg != nullptr) {
			if (ogg->playing()) {
				ogg->setVolume(volume, true);
			}
		}
	}
	void SoundSource::addEffect(SoundEffect* soundEffect) {
		effects.push_back(soundEffect);
	}
}