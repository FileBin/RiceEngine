#pragma once
#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/OggStream.h>
#include <GameEngine/AlDevice.h>

namespace Game {
	class SoundManager : private AlDevice{
	public:
		SoundManager();
		~SoundManager();

		void play_music(const char* name, bool force);
		void play_sound(const char* name, float volume);
		void setMusicVolume(float volume);
	private:
		void playOggStream(OggStream ogg);
		void music_thread();
		void sound_thread(std::string path, float volume);
		void list_audio_devices(const ALCchar* devices);
	};
}