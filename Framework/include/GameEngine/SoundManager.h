#pragma once
#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/SoundStream.h>
#include <GameEngine/AlDevice.h>
#include <GameEngine/Vectors/Vector3f.h>
#include <GameEngine/Camera.h>

namespace Game {
	class SoundManager : private AlDevice{
	public:
		SoundManager(SmartPtr<Camera> cam);
		~SoundManager();

		void play_music(const std::string name, bool force);
		std::weak_ptr<SoundStream> play_raw(FrequencyFunc f, double beginning, double end, float volume, Vector3f position);
		std::weak_ptr<SoundStream> play_sound(const std::string name, float volume, Vector3f position);
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

		SoundManager* instance = nullptr;
		SmartPtr<Camera> camera;
		ALCdevice* openALDevice;

		ALCboolean contextMadeCurrent;
		ALCcontext* openALContext;

		std::shared_ptr<OggStream> current_music_stream;
		float musicVolume = 1;
		std::string nextMusic;
	};
}