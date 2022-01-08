#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exceptions.h>
#include <GameEngine/Util.h>

#include <thread>
#include <ppltasks.h>

namespace Game {

	SoundManager::SoundManager(SmartPtr<Camera> cam) {
		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
		openALDevice = alcOpenDevice(nullptr); // default device
		if (!openALDevice) {
			THROW_OPENAL_EXCEPTION("Sound device initialization failed!");
		}
		if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext) {
			THROW_OPENAL_EXCEPTION("Could not create openAL context!");
		}
		contextMadeCurrent = false;
		if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
			|| contextMadeCurrent != ALC_TRUE) {
			THROW_OPENAL_EXCEPTION("Could not make openAL context context current!");
		}

		camera = cam;
		concurrency::create_task([this]() {music_thread(); });
		concurrency::create_task([this]() {update_thread(); });

		Log::log(Log::INFO, L"OpenAL init success");
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

	void SoundManager::update_thread() {
		auto cam = camera;
		while (true) {
			setListenerPosition(cam->position);
			setListenerOrientation(cam->rotation * Vector3::forward, cam->rotation * Vector3::up);
			Sleep(30);
		}
	}

	void SoundManager::setListenerPosition(Vector3f position) {
		alListener3f(AL_VELOCITY, position.x - prevPos.x, position.y - prevPos.y, position.z - prevPos.z);
		prevPos.x = position.x;
		prevPos.y = position.y;
		prevPos.z = position.z;
		alListener3f(AL_POSITION, position.x, position.y, position.z);
	}

	void SoundManager::setListenerOrientation(Vector3f at, Vector3f up) {
		ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
		alListenerfv(AL_ORIENTATION, listenerOri);
	}

	void SoundManager::list_audio_devices(const ALCchar* devices)
	{
		const ALCchar* device = devices, * next = devices + 1;
		size_t len = 0;

		Log::log(Log::INFO, L"OpenAL Devices list:");
		Log::log(Log::INFO, L"----------");
		while (device && *device != '\0' && next && *next != '\0') {
			Log::log(Log::INFO, Util::Utf8ToWstring(std::string(device)));
			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}
		Log::log(Log::INFO, L"----------\n");
	}

	void SoundManager::playSoundStream(SmartPtr<SoundStream> ogg) {
		if (!ogg->playback())
			throw std::wstring(L"Ogg refused to play");
		while (ogg->update())
		{
			if (!ogg->playing())
			{
				if (!ogg->playback())
					throw std::wstring(L"Ogg abruptly stopped");
				else
					throw std::wstring(L"Ogg stream was interrupted");
			}
			Sleep(1);
		}
		ogg->release();
	}

	void SoundManager::music_thread() {
		while (true) {
			if (nextMusic.size() > 0) {
				SmartPtr<SoundStream> ogg = new SoundStream();
				try {
					ogg->playOgg(nextMusic);
					ogg->setVolume(musicVolume, false);
					current_music_stream = ogg;
					nextMusic.clear();
					playSoundStream(ogg);
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

	void SoundManager::sound_thread(SmartPtr<SoundStream> ogg, std::string path, float volume, Vector3f pos) {
		try {
			ogg->playOgg(path);
			ogg->setVolume(volume, true);
			ogg->setPosition(pos);
			playSoundStream(ogg);
		}
		catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	void SoundManager::raw_sound_thread(SmartPtr<SoundStream> ogg, FrequencyFunc f, dbl beginning, dbl end, float volume, Vector3f pos) {
		try {
			ogg->playRaw(f, beginning, end);
			ogg->setVolume(volume, true);
			ogg->setPosition(pos);
			playSoundStream(ogg);
		}
		catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	SmartPtr<SoundStream> SoundManager::play_sound(const std::string name, float volume, Vector3f pos) {
		SmartPtr<SoundStream> ogg = new SoundStream();;
		concurrency::create_task([&, ogg]() {sound_thread(ogg, "sfx/" + name + ".ogg", volume, pos); });
		return ogg;
	 }

	SmartPtr<SoundStream> SoundManager::play_raw(FrequencyFunc f, double beginning, double end, float volume, Vector3f pos) {
		 SmartPtr<SoundStream> ogg = new SoundStream();
		 concurrency::create_task([&, ogg, f]() {raw_sound_thread(ogg, f, beginning, end, volume, pos); });
		 return ogg;
	 }

	void SoundManager::setMusicVolume(float volume) {
		musicVolume = volume;
		if (!current_music_stream.IsNull()) {
			current_music_stream->setVolume(volume, false);
		}
	}

	void SoundManager::play_music(const std::string name, bool force) {
		if (!current_music_stream.IsNull() && force) {
			current_music_stream->setVolume(0, false);
			current_music_stream->closeOnNoVolume(true);
		}
		nextMusic = "music/" + name + ".ogg";
	}
}