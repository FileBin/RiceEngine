#include "pch.h"
#include <GameEngine/Components/SoundSource.h>

namespace Game {
	void SoundSource::OnInit() {
		scene = &GetSceneObject().GetScene();
		soundManager = &scene->GetSoundManager();
		ogg = nullptr;
		Initialize();
	}
	SoundSource::~SoundSource() {};
	Scene& SoundSource::GetScene() { return *scene; }
	SceneRender& SoundSource::GetRender() { return GetScene().GetRender(); }
	Engine& SoundSource::GetEngine() { return GetScene().GetEngine(); }
	void SoundSource::play(char* name, float volume, Vector3f position) {
		ogg = soundManager->play_sound(name, volume, position);
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
}