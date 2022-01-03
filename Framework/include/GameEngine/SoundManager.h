#pragma once
#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/AlDevice.h>

namespace Game {
	class SoundManager : private AlDevice{
	public:
		SoundManager();
		~SoundManager();

		void play(const char* path);
	private:
		void play_thread(const char* path);
		void list_audio_devices(const ALCchar* devices);
	};
}