#include "pch.h"
#include <GameEngine/Components/SoundSource.h>

namespace Game {
	void SoundSource::OnInit() {
		scene = &GetSceneObject().GetScene();
		soundManager = &scene->GetSoundManager();
		ogg = nullptr;
		transform = GetSceneObject().GetComponents<Transform>()[0];
	}
	SoundSource::~SoundSource() {};
	Scene& SoundSource::GetScene() { return *scene; }
	SceneRender& SoundSource::GetRender() { return GetScene().GetRender(); }
	Engine& SoundSource::GetEngine() { return GetScene().GetEngine(); }
	void SoundSource::Update() {
		setPosition(transform->position);
	}
	void SoundSource::play(char* name, float volume, Vector3f position) {
		ogg = soundManager->play_sound(name, volume, position).lock();
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