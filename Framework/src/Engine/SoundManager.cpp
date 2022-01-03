#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exception.h>
#include <GameEngine/Util.h>
#include <GameEngine/OggStream.h>

#include <thread>
#include <ppltasks.h>

namespace Game {

	SoundManager* instance = nullptr;
	ALCdevice* openALDevice;

	ALCboolean contextMadeCurrent;
	ALCcontext* openALContext;

	std::vector<OggStream> streams;
	OggStream stream;

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

	void SoundManager::play_thread(const char* path) {
		OggStream ogg;
		try {
			ogg.open(path);
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
			}
			ogg.release();
		} catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	void SoundManager::play(const char* path) {
		concurrency::create_task([&]() {play_thread(path); });
	}
}