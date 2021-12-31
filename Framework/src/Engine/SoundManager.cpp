#include "pch.h"
#include <GameEngine/SoundManager.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util/exception.h>

#include <al/al.h>
#include <al/alc.h>
#include <vorbis/vorbisfile.h>

namespace Game {
	
	SoundManager* instance = nullptr;
	ALCdevice* openALDevice;

	ALCboolean contextMadeCurrent;
	ALCcontext* openALContext;

	SoundManager* SoundManager::Init() {
		if (instance == nullptr) {
			instance = new SoundManager();
			return instance;
		}
		throw Game::exception("Sound is already initialized!", 16, L"SoundManager.cpp : SoundManager* SoundManager::Init()");
	}

	SoundManager::SoundManager() {
		openALDevice = alcOpenDevice(nullptr); // default device
		if (!openALDevice)
		{
			throw Game::exception("Sound device initialization failed!", 27, L"SoundManager.cpp : SoundManager::SoundManager()");
		}
		openALContext;
		if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext)
		{
			throw Game::exception("Could not create openAL context!", 32, L"SoundManager.cpp : SoundManager::SoundManager()");
		}
		contextMadeCurrent = false;
		if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
			|| contextMadeCurrent != ALC_TRUE)
		{
			throw Game::exception("Could not make openAL context context current!", 39, L"SoundManager.cpp : SoundManager::SoundManager()");
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

		if ((err = fopen_s(&fp, path, "r")) != 0) {
			fprintf(stderr, "cannot open file '%s': %d\n",
				path, err);
			goto exit;
		}

		// make a handle
		sound = (ALuint*)malloc(1 * sizeof(ALuint));
		if (sound == 0) {
			fprintf(stderr, "Out of memory.");
			goto exit;
		}

		// make a buffer
		alGenBuffers(1, sound);

		// check for errors
		if ((error = alGetError()) != AL_NO_ERROR) {
			fprintf(stderr, "Failed to generate sound buffer %d\n", error);
			//goto exit;
		}

		// open the ogg vorbis file. This is a must on windows, do not use ov_open.
	  // set OV_CALLBACKS_NOCLOSE else it will close your fp when ov_close() is reached, which is fine.
		if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
			fprintf(stderr, "Stream is not a valid OggVorbis stream!\n");
			goto exit;
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
			fprintf(stderr, "Out of memory.\n");
			goto exit;
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
				puts("Faulty ogg file :o"); // use https://xiph.org/vorbis/doc/vorbisfile/ov_read.html for handling enums
		}

		// send data to openal, vi->rate is your freq in Hz, dont assume 44100
		alBufferData(*sound, format, pcmout, data_len, vi->rate);
		if ((error = alGetError()) != AL_NO_ERROR) {
			printf("Failed to send audio information buffer to OpenAL! 0x%06x\n", error);
			goto exit;
		}

		// free your resources >:(
		free(pcmout);
		fclose(fp);
		ov_clear(&vf);
		return sound;

	exit:
		free(pcmout);
		free(sound);
		fclose(fp);
		ov_clear(&vf);
		return 0;
	}

}