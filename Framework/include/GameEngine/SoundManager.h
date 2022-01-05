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
		SoundManager(Camera *cam);
		~SoundManager();

		void play_music(const char* name, bool force);
		OggStream* play_sound(const char* name, float volume, Vector3f position);
		void setMusicVolume(float volume);

		void update_thread();

		void setListenerPosition(Vector3f position);
		void setListenerOrientation(Vector3f at, Vector3f up);

	private:
		void playOggStream(OggStream *ogg);
		void music_thread();
		void sound_thread(OggStream *ogg, std::string path, float volume, Vector3f position);
		void list_audio_devices(const ALCchar* devices);
	};
}