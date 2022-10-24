#pragma once
//#include "ALHelper.hpp"
#include "../stdafx.hpp"

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include "SoundStream.hpp"
//#include <GameEngine/Camera.h>

NSP_AL_BEGIN

class SoundManager
{ // : private AlDevice{
public:
	SoundManager(RefPtr<Camera> cam);
	~SoundManager();

	void playMusic(const std::string name, bool force);
	RefPtr<SoundStream> playRaw(FrequencyFunc f, double beginning, double end, float volume, Vector3f position /*, std::vector<SoundEffect*> effects = {}*/);
	RefPtr<SoundStream> playSound(const std::string name, float volume, Vector3f position /*, std::vector<SoundEffect*> effects*/);
	void setMusicVolume(float volume);

	void updateThread();

private:
	void play_sound_stream(RefPtr<SoundStream> ogg);
	void music_thread();
	void sound_thread(RefPtr<SoundStream> ogg, std::string path, float volume, Vector3f position /*, std::vector<SoundEffect*> effects*/);
	void raw_sound_thread(RefPtr<SoundStream> ogg, FrequencyFunc f, dbl beginning, dbl end, float volume, Vector3f pos /*, std::vector<SoundEffect*> effects = {}*/);

	void set_listener_position(Vector3f position);

	Vector3f prevPos = {};

	// static SoundManager* instance;
	bool init = false;
	RefPtr<std::thread> updateThread, musicThread;
	RefPtr<Camera> camera;

	RefPtr<SoundStream> current_music_stream;
	float musicVolume = 1;
	std::string nextMusic;
};

NSP_AL_END