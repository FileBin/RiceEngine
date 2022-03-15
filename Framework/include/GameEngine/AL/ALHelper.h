#pragma once

#include <AL\al.h>
#include <AL\alc.h>

#include "AlDevice.h"
#include <GameEngine\Vectors\Vector3.h>
#include <GameEngine\Vectors\Vector3f.h>
#include <GameEngine\Vectors\Quaternion.h>

namespace Game {
	class AL {
	public:
		class SoundBuffer {
		private:
			ALuint buffers[2];
			ALuint source;
			ALenum format;

			bool hasEffects;

		public:
			SoundBuffer();
			~SoundBuffer();

			void Play();
			bool IsPlaying();

			/// <summary>
			/// Copies data to the audio buffer
			/// </summary>
			/// <param name="data"> - data of sound</param>
			/// <param name="sampleRate"> - sample rate of sound </param>
			/// <param name="channel"> - channel index, 0 - left, 1 - right</param>
			void EmplaceData(data_t data, int sampleRate = 44100, int channel = 0);

			ALenum GetFormat() { return format; }
			void SetFormat(ALenum _format) { format = _format; }
			
			void SetVolume(float volume);
			void SetLooping(bool looping);
			void SetPitch(float pitch);

			void SetPosition(Vector3f position);
			void SetVelocity(Vector3f velocity);
		};
	private:
		static SmartPtr<AL> instance;

		AlDevice* device;
		ALCdevice* openALDevice;

		ALCboolean contextMadeCurrent;
		ALCcontext* openALContext;

		bool init;
		void _init();
		void _listAudioDevices(const ALCchar* devices);

		void _close();
	public:
		static void Init();
		static void Close();

		static void SetListenerPosition(Vector3f position);
		static void SetListenerVelocity(Vector3f velocity);
		static void SetListenerOrientation(Quaternion q);

#pragma region Properties
		static bool IsInitialized() {
			if (instance.IsNull()) return false;
			return instance->init;
		}
#pragma endregion
	};
}
