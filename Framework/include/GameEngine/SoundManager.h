#pragma once
#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/OggStream.h>
#include <GameEngine/AlDevice.h>
#include <GameEngine/Vectors/Vector3f.h>
#include <GameEngine/Camera.h>

namespace Game {
	class SoundManager : private AlDevice{
	public:
		SoundManager(std::shared_ptr<Camera> cam);
		~SoundManager();

		void play_music(const std::string name, bool force);
		std::shared_ptr<OggStream> play_sound(const std::string name, float volume, Vector3f position);
		void setMusicVolume(float volume);

		void update_thread();

		void setListenerPosition(Vector3f position);
		void setListenerOrientation(Vector3f at, Vector3f up);

	private:
		void playOggStream(std::shared_ptr<OggStream> ogg);
		void music_thread();
		void sound_thread(std::shared_ptr<OggStream> ogg, std::string path, float volume, Vector3f position);
		void list_audio_devices(const ALCchar* devices);

		Vector3f prevPos = {};
	};
}