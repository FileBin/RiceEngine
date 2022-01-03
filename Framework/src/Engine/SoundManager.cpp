#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exception.h>
#include <GameEngine/Util.h>

#include <vorbis/vorbisfile.h>

namespace Game {
	
	SoundManager* instance = nullptr;
	ALCdevice* openALDevice;

	ALCboolean contextMadeCurrent;
	ALCcontext* openALContext;

	SoundManager::SoundManager() {
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
		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
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

	bool SoundManager::check_al_errors(const std::wstring& filename, const std::uint_fast32_t line)
	{
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			Log::log(Log::ERR, L"Audio error occured");
			Log::log(Log::ERR, filename);
			Log::log(Log::ERR, std::to_wstring(line));
			switch (error)
			{
			case AL_INVALID_NAME:
				Log::log(Log::ERR, L"AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
				break;
			case AL_INVALID_ENUM:
				Log::log(Log::ERR, L"AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
				break;
			case AL_INVALID_VALUE:
				Log::log(Log::ERR, L"AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
				break;
			case AL_INVALID_OPERATION:
				Log::log(Log::ERR, L"AL_INVALID_OPERATION: the requested operation is not valid");
				break;
			case AL_OUT_OF_MEMORY:
				Log::log(Log::ERR, L"AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
				break;
			default:
				Log::log(Log::ERR, L"UNKNOWN AL ERROR: " + std::to_wstring(error));
			}
			return false;
		}
		return true;
	}

	bool SoundManager::check_alc_errors(const std::wstring& filename, const std::uint_fast32_t line, ALCdevice* device)
	{
		ALCenum error = alcGetError(device);
		if (error != ALC_NO_ERROR)
		{
			Log::log(Log::ERR, L"Audio error occured");
			Log::log(Log::ERR, filename);
			Log::log(Log::ERR, std::to_wstring(line));
			switch (error)
			{
			case ALC_INVALID_VALUE:
				Log::log(Log::ERR, L"ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
				break;
			case ALC_INVALID_DEVICE:
				Log::log(Log::ERR, L"ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
				break;
			case ALC_INVALID_CONTEXT:
				Log::log(Log::ERR, L"ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
				break;
			case ALC_INVALID_ENUM:
				Log::log(Log::ERR, L"ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
				break;
			case ALC_OUT_OF_MEMORY:
				Log::log(Log::ERR, L"ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
				break;
			default:
				Log::log(Log::ERR, L"UNKNOWN ALC ERROR: " + std::to_wstring(error));
			}
			return false;
		}
		return true;
	}

	void SoundManager::list_audio_devices(const ALCchar* devices)
	{
		const ALCchar* device = devices, * next = devices + 1;
		size_t len = 0;

		Log::log(Log::INFO, L"Devices list:\n");
		Log::log(Log::INFO, L"----------\n");
		while (device && *device != '\0' && next && *next != '\0') {
			Log::log(Log::INFO, Util::Utf8ToWstring(std::string(device)));
			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}
		Log::log(Log::INFO, L"----------\n");
	}

	bool SoundManager::check_al_errors(const char* filename, const std::uint_fast32_t line)
	{
		return check_al_errors(Util::Utf8ToWstring(std::string(filename)), line);
	}

	bool SoundManager::check_alc_errors(const char* filename, const std::uint_fast32_t line, ALCdevice* device)
	{
		return check_alc_errors(Util::Utf8ToWstring(std::string(filename)), line, device);
	}

	void SoundManager::playOgg(const char* path) {
		play(sound_load_ogg(path));
	}

	void SoundManager::play(ALuint* sound) {
		//create source
		ALuint source;
		alCall(alGenSources, (ALuint)1, &source);
		alCall(alSourcef, source, AL_PITCH, 1);
		alCall(alSourcef, source, AL_GAIN, 1.0f);
		alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
		alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
		alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
		alCall(alSourcei, source, AL_BUFFER, *sound);

		alCall(alSourcePlay, source);

		ALint state = AL_PLAYING;

		while (state == AL_PLAYING)
		{
			alCall(alGetSourcei, source, AL_SOURCE_STATE, &state);
		}

		alCall(alDeleteSources, 1, &source);
		alCall(alDeleteBuffers, 1, sound);		
	}

	ALuint* SoundManager::sound_load_ogg(const char* path) {
		ALenum error = 0;
		ALuint* sound = 0;
		FILE* fp = 0;
		errno_t err;
		OggVorbis_File vf;
		vorbis_info* vi = 0;
		ALenum format = 0;
		short* pcmout = 0;

		size_t data_len = 0;

		if ((err = fopen_s(&fp, path, "rb")) != 0) {
			throw Game::exception("cannot open audio file (line = error_code)", err, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
		}

		// make a handle
		sound = (ALuint*)malloc(1 * sizeof(ALuint));
		if (sound == 0) {
			throw Game::exception("Cannot create audio, out of memory.", 194, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
		}

		// make a buffer
		if (!alCall(alGenBuffers, 1, sound)) {
			throw Game::exception("Failed to generate sound buffer (line = error_code)", error, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
		}

		// open the ogg vorbis file. This is a must on windows, do not use ov_open.
	    // set OV_CALLBACKS_NOCLOSE else it will close your fp when ov_close() is reached, which is fine.
		if ((error = ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE)) != 0) {
			switch (error)
			{
			case OV_EREAD:
				throw Game::exception("A read from media returned an error", 208, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
				break;
			case OV_ENOTVORBIS:
				throw Game::exception("Bitstream does not contain any Vorbis data", 211, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
				break;
			case OV_EVERSION:
				throw Game::exception("Vorbis version mismatch", 214, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
				break;
			case OV_EBADHEADER:
				throw Game::exception("Invalid Vorbis bitstream header", 217, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
				break;
			case OV_EFAULT:
				throw Game::exception("Internal logic fault; indicates a bug or heap/stack corruption", 220, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
				break;
			}
		}

		// fill vi with a new ogg vorbis info struct, determine audio format
		// audio format will always been a length of 16bits, vi->channels determines mono or stereo
		vi = ov_info(&vf, -1);
		format = vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		// data_len is the amount of data to read, allocate said data space
		// this is calculated by (samples * channels * 2 (aka 16bits))
		data_len = ov_pcm_total(&vf, -1) * vi->channels * 2;
		pcmout = (short*)malloc(data_len);
		if (pcmout == 0) {
			throw Game::exception("Cannot create audio, out of memory.", 235, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
		}

		// fill pcmout buffer with ov_read data samples
		// you can't just slap data_len in place of 4096, it doesn't work that way
		// 0 is endianess, 0 for little, 1 for big
		// 2 is the data type short's size, mine is 2
		// 1 is the signedness you want, I want short not unsigned short (for openal) so 1
		for (size_t size = 0, offset = 0, sel = 0;
			(size = ov_read(&vf, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)&sel)) != 0;
			offset += size) {
			if (size < 0)
				throw Game::exception("Faulty ogg file :o", 247, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)"); // use https://xiph.org/vorbis/doc/vorbisfile/ov_read.html for handling enums
		}

		// send data to openal, vi->rate is your freq in Hz, dont assume 44100
		if (!alCall(alBufferData, *sound, format, pcmout, data_len, vi->rate)) {
			throw Game::exception("Failed to send audio information buffer to OpenAL! (line = error_code)", error, L"SoundManager.cpp: ALuint* SoundManager::sound_load_ogg(const char* path)");
		}

		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return sound;
	}

}