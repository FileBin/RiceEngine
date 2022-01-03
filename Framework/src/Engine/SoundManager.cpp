#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exception.h>
#include <GameEngine/Util.h>

#include <thread>
#include <ppltasks.h>

namespace Game {

	SoundManager* instance = nullptr;
	ALCdevice* openALDevice;

	ALCboolean contextMadeCurrent;
	ALCcontext* openALContext;

	OggStream* current_music_stream;
	float musicVolume = 1;
	std::string nextMusic;

	SoundManager::SoundManager() {
		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
		openALDevice = alcOpenDevice(nullptr); // default device
		if (!openALDevice)
		{
			throw Game::exception("Sound device initialization failed!", 21, L"SoundManager.cpp : SoundManager::SoundManager()");
		}
		openALContext;
		if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext)
		{
			throw Game::exception("Could not create openAL context!", 26, L"SoundManager.cpp : SoundManager::SoundManager()");
		}
		contextMadeCurrent = false;
		if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
			|| contextMadeCurrent != ALC_TRUE)
		{
			throw Game::exception("Could not make openAL context context current!", 32, L"SoundManager.cpp : SoundManager::SoundManager()");
		}
		concurrency::create_task([this]() {music_thread(); });
	}

	SoundManager::~SoundManager() {
		if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr))
		{
			Log::log(Log::WARNING, L"Error while destroying openAL context");
		}

		if (!alcCall(alcDestroyContext, openALDevice, openALContext))
		{
			Log::log(Log::WARNING, L"Error while destroying openAL context");
		}

		ALCboolean closed;
		alcCall(alcCloseDevice, closed, openALDevice, openALDevice);
	}

	void SoundManager::list_audio_devices(const ALCchar* devices)
	{
		const ALCchar* device = devices, * next = devices + 1;
		size_t len = 0;

		Log::log(Log::INFO, L"Devices list:");
		Log::log(Log::INFO, L"----------");
		while (device && *device != '\0' && next && *next != '\0') {
			Log::log(Log::INFO, Util::Utf8ToWstring(std::string(device)));
			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}
		Log::log(Log::INFO, L"----------\n");
	}

	void SoundManager::playOggStream(OggStream ogg) {
		if (!ogg.playback())
			throw std::wstring(L"Ogg refused to play");
		while (ogg.update())
		{
			if (!ogg.playing())
			{
				if (!ogg.playback())
					throw std::wstring(L"Ogg abruptly stopped");
				else
					throw std::wstring(L"Ogg stream was interrupted");
			}
			Sleep(1);
		}
		ogg.release();
	}

	void SoundManager::music_thread() {
		while (true) {
			if (nextMusic.size() > 0) {
				OggStream ogg;
				try {
					ogg.open(nextMusic);
					ogg.setVolume(musicVolume, false);
					current_music_stream = &ogg;
					nextMusic.clear();
					playOggStream(ogg);
					current_music_stream = nullptr;
				}
				catch (std::wstring e) {
					current_music_stream = nullptr;
					Log::log(Log::ERR, e);
				}
			}
			else {
				Sleep(100);
			}
		}
	}

	void SoundManager::sound_thread(std::string path, float volume) {
		OggStream ogg;
		try {
			ogg.open(path);
			ogg.setVolume(volume, true);
			playOggStream(ogg);
		}
		catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	void SoundManager::play_sound(const char* name, float volume) {
		concurrency::create_task([&]() {sound_thread("sfx/" + std::string(name) + ".ogg", volume); });
	}

	void setMusicVolume(float volume) {
		musicVolume = volume;
		if (current_music_stream != nullptr) {
			current_music_stream->setVolume(volume, false);
		}
	}

	void SoundManager::play_music(const char* name, bool force) {
		if (current_music_stream != nullptr && force) {
			current_music_stream->setVolume(0, false);
			concurrency::create_task([&]() {Sleep(1000); nextMusic = "music/" + std::string(name) + ".ogg"; });
		}
		else {
			nextMusic = "music/" + std::string(name) + ".ogg";
		}
	}
}