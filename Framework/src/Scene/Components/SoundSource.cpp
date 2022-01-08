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
		ogg = soundManager->play_sound(name, volume, transform->position);
		ogg->applyEffectChain(&effects);
	}
	void SoundSource::play(FrequencyFunc f, dbl beginning, dbl end, float volume) {
		ogg = soundManager->play_raw(f, beginning, end, volume, transform->position);
		ogg->applyEffectChain(&effects);
	}
	void SoundSource::setPosition(Vector3f position) {
		if (!ogg.IsNull()) {
			if (ogg->playing()) {
				ogg->setPosition(position);
			}
		}
	}
	void SoundSource::setVolume(float volume) {
		if (!ogg.IsNull()) {
			if (ogg->playing()) {
				ogg->setVolume(volume, true);
			}
		}
	}
	void SoundSource::addEffect(SoundEffect* soundEffect) {
		effects.push_back(soundEffect);
	}
	SoundEffect* SoundSource::addEffect(ALenum effect) {
		SoundEffect* eff = new SoundEffect(effect);
		effects.push_back(eff);
		return eff;
	}
	void SoundSource::setLastAddedEffectPropertyf(ALenum property, ALfloat value) {
		effects.at(effects.size() - 1)->setPropertyf(property, value);
	};
	void SoundSource::setLastAddedEffectPropertyi(ALenum property, ALint value) {
		effects.at(effects.size() - 1)->setPropertyi(property, value);
	};
	void SoundSource::setLastAddedEffectPropertyfv(ALenum property, const ALfloat* values) {
		effects.at(effects.size() - 1)->setPropertyfv(property, values);
	};
	void SoundSource::setLastAddedEffectPropertyiv(ALenum property, const ALint* values) {
		effects.at(effects.size() - 1)->setPropertyiv(property, values);
	};
	void SoundSource::setLastAddedEffectEfxReverbPreset(EFXEAXREVERBPROPERTIES preset) {
		effects.at(effects.size() - 1)->setEfxReverbPreset(preset);
	};
}