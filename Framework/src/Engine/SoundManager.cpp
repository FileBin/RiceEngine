#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exceptions.h>
#include <GameEngine/Util.h>

#include <GameEngine\Core.h>

namespace Game {

	//SoundManager* SoundManager::instance = nullptr;

	SoundManager::SoundManager(SmartPtr<Camera> cam) {
		if (!AL::IsInitialized())
			THROW_OPENAL_EXCEPTION("AL is not initialized!");

		camera = cam;

		init = true;

		musicThread = Core::RunThread<void()>([this] () { music_thread(); });
		updateThread = Core::RunThread<void()>([this] () { update_thread(); });

		Log::log(Log::INFO, L"OpenAL init success");
	}

	SoundManager::~SoundManager() {
		init = false;

		musicThread->join();
		musicThread.Release();

		updateThread->join();
		updateThread.Release();
	}

	void SoundManager::update_thread() {
		auto cam = camera;
		while (init) {
			if (cam.IsNull())
				break;
			setListenerPosition(cam->position);
			//setListenerOrientation(cam->rotation * Vector3::forward, cam->rotation * Vector3::down);
			AL::SetListenerOrientation(cam->rotation);
			Sleep(30);
		}
	}

	void SoundManager::setListenerPosition(Vector3f position) {
		AL::SetListenerPosition(position);
		AL::SetListenerVelocity(position - prevPos);
		prevPos.x = position.x;
		prevPos.y = position.y;
		prevPos.z = position.z;
	}

	void SoundManager::playSoundStream(SmartPtr<SoundStream> ogg) {
		if (!ogg->Play())
			throw std::wstring(L"Ogg refused to play");
		while (ogg->update() && init)
		{
			if (!ogg->IsPlaying())
			{
				if (!ogg->Play())
					throw std::wstring(L"Ogg abruptly stopped");
				else
					throw std::wstring(L"Ogg stream was interrupted");
			}
			Sleep(1);
		}
		ogg->release();
	}

	void SoundManager::music_thread() {
		while (init) {
			if (nextMusic.size() > 0) {
				SmartPtr<SoundStream> ogg = new SoundStream();
				try {
					ogg->LoadOgg(nextMusic);
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

	void SoundManager::sound_thread(SmartPtr<SoundStream> ogg, std::string path, float volume, Vector3f pos/*, std::vector<SoundEffect*> effects*/) {
		try {
			ogg->LoadOgg(path);
			ogg->setVolume(volume, true);
			ogg->setPosition(pos);
			//if (!effects.Clear()) {
			//	ogg->applyEffectChain(effects);
			//}
			playSoundStream(ogg);
		}
		catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	void SoundManager::raw_sound_thread(SmartPtr<SoundStream> ogg, FrequencyFunc f, dbl beginning, dbl end, float volume, Vector3f pos/*, std::vector<SoundEffect*> effects*/) {
		try {
			ogg->LoadRaw(f, beginning, end);
			ogg->setVolume(volume, true);
			ogg->setPosition(pos);
			//if (!effects.Clear()) {
			//	ogg->applyEffectChain(effects);
			//}
			playSoundStream(ogg);
		}
		catch (std::wstring e) {
			Log::log(Log::ERR, e);
		}
	}

	SmartPtr<SoundStream> SoundManager::play_sound(const std::string name, float volume, Vector3f pos/*, std::vector<SoundEffect*> effects*/) {
		SmartPtr<SoundStream> ogg = new SoundStream();
		concurrency::create_task([&, ogg, name, volume, pos/*, effects*/]() {sound_thread(ogg, "./sfx/" + name + ".ogg", volume, pos/*, effects*/); });
		return ogg;
	 }

	SmartPtr<SoundStream> SoundManager::play_raw(FrequencyFunc f, double beginning, double end, float volume, Vector3f pos/*, std::vector<SoundEffect*> effects*/) {
		 SmartPtr<SoundStream> ogg = new SoundStream();
		 concurrency::create_task([&, ogg, f, beginning, end, volume, pos/*, effects*/]() {raw_sound_thread(ogg, f, beginning, end, volume, pos/*, effects*/); });
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