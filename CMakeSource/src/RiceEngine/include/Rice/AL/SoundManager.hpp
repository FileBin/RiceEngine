#pragma once
#include "AL\ALHelper.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/SoundStream.h>
//#include <GameEngine/AlDevice.h>
#include <GameEngine/Vectors/Vector3f.h>
#include <GameEngine/Camera.h>

namespace Game {
	class SoundManager {// : private AlDevice{
	public:
		SoundManager(SmartPtr<Camera> cam);
		~SoundManager();

		void play_music(const std::string name, bool force);
		SmartPtr<SoundStream> play_raw(FrequencyFunc f, double beginning, double end, float volume, Vector3f position/*, std::vector<SoundEffect*> effects = {}*/);
		SmartPtr<SoundStream> play_sound(const std::string name, float volume, Vector3f position/*, std::vector<SoundEffect*> effects*/);
		void setMusicVolume(float volume);

		void update_thread();

	private:
		void playSoundStream(SmartPtr<SoundStream> ogg);
		void music_thread();
		void sound_thread(SmartPtr<SoundStream> ogg, std::string path, float volume, Vector3f position/*, std::vector<SoundEffect*> effects*/);
		void raw_sound_thread(SmartPtr<SoundStream> ogg, FrequencyFunc f, dbl beginning, dbl end, float volume, Vector3f pos/*, std::vector<SoundEffect*> effects = {}*/);

		void setListenerPosition(Vector3f position);

		Vector3f prevPos = {};

		//static SoundManager* instance;
		bool init = false;
		SmartPtr<std::thread> updateThread, musicThread;
		SmartPtr<Camera> camera;

		SmartPtr<SoundStream> current_music_stream;
		float musicVolume = 1;
		std::string nextMusic;
	};
}