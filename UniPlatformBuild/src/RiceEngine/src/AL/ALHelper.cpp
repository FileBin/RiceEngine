#include "pch.h"

#include <GameEngine\AL\ALHelper.h>

#define DeviceAlCall instance->device->alCall
namespace Game {
	SmartPtr<AL> AL::instance;
}
void Game::AL::Init() {
	instance = new AL();
	instance->_init();
}

void Game::AL::Close() {
	instance->_close();
}

Game::AL::SoundBuffer::SoundBuffer() {
	DeviceAlCall(alGenBuffers, 2, buffers);
	DeviceAlCall(alGenSources, 1, &source);
	DeviceAlCall(alSourcef, source, AL_GAIN, 0);
}

Game::AL::SoundBuffer::~SoundBuffer() {
	DeviceAlCall(alSourceStop, source);

	int queued;

	DeviceAlCall(alGetSourcei, source, AL_BUFFERS_QUEUED, &queued);

	while (queued--) {
		ALuint buffer;
		DeviceAlCall(alSourceUnqueueBuffers, source, 1, &buffer);
	}

	DeviceAlCall(alDeleteSources, 1, &source);
	DeviceAlCall(alDeleteBuffers, 2, buffers);

	//if (hasEffects) {
	//	alCall(alDeleteFilters, 1, &filter);
	//}
}

void Game::AL::SoundBuffer::EmplaceData(data_t data, int sampleRate, int channel) {
	DeviceAlCall(alBufferData, buffers[channel], format, data.data(), data.size(), sampleRate);
}

void Game::AL::SoundBuffer::SetVolume(float volume) {
	DeviceAlCall(alSourcef, source, AL_GAIN, volume);
}

void Game::AL::SoundBuffer::SetLooping(bool looping) {
	DeviceAlCall(alSourcei, source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void Game::AL::SoundBuffer::SetPitch(float pitch) {
	DeviceAlCall(alSourcef, source, AL_PITCH, pitch);
}

void Game::AL::SoundBuffer::SetPosition(Vector3f position) {
	DeviceAlCall(alSource3f, source, AL_POSITION, position.x, position.y, position.z);
}

void Game::AL::SoundBuffer::SetVelocity(Vector3f velocity) {
	DeviceAlCall(alSource3f, source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void Game::AL::SoundBuffer::Play() {
	if (IsPlaying())return;
	DeviceAlCall(alSourceQueueBuffers, source, 2, buffers);
	DeviceAlCall(alSourcePlay, source);
}

bool Game::AL::SoundBuffer::IsPlaying() {
	if (source == 0) {
		return false;
	}

	ALenum rs_state;

	DeviceAlCall(alGetSourcei, source, AL_SOURCE_STATE, &rs_state);

	return (rs_state == AL_PLAYING);
}

void Game::AL::SetListenerPosition(Vector3f position) {
	instance->device->
		alCall(alListener3f, AL_POSITION, position.x, position.y, position.z);
}

void Game::AL::SetListenerVelocity(Vector3f velo) {
	instance->device->
		alCall(alListener3f, AL_VELOCITY, velo.x, velo.y, velo.z);
}

void Game::AL::SetListenerOrientation(Quaternion q) {
	Vector3 up = q * Vector3::down, at = q * Vector3::forward;
	ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
	instance->device->alCall(alListenerfv, AL_ORIENTATION, listenerOri);
}

void Game::AL::_init() {
	device = new AlDevice();

	_listAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	openALDevice = alcOpenDevice(nullptr); // default device
	if (!openALDevice) {
		THROW_OPENAL_EXCEPTION("Sound device initialization failed!");
	}
	if (!device->alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext) {
		THROW_OPENAL_EXCEPTION("Could not create openAL context!");
	}
	contextMadeCurrent = false;
	if (!device->alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
		|| contextMadeCurrent != ALC_TRUE) {
		THROW_OPENAL_EXCEPTION("Could not make openAL context context current!");
	}
	init = true;
}

void Game::AL::_listAudioDevices(const ALCchar* devices) {
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

void Game::AL::_close() {
	if (!device->alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr)) {
		Log::log(Log::WARNING, L"Error while destroying openAL context");
	}

	if (!device->alcCall(alcDestroyContext, openALDevice, openALContext)) {
		Log::log(Log::WARNING, L"Error while destroying openAL context");
	}

	ALCboolean closed;
	device->alcCall(alcCloseDevice, closed, openALDevice, openALDevice);
}