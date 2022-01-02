#pragma once
#define AL_LIBTYPE_STATIC 
#include <al/al.h>
#include <al/alc.h>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace Game {
	class SoundManager {
	public:
		static SoundManager* Init();
		SoundManager();
		~SoundManager();

		void playOgg(const char* path);
	private:
		bool check_al_errors(const std::wstring& filename, const std::uint_fast32_t line);
		bool check_alc_errors(const std::wstring& filename, const std::uint_fast32_t line, ALCdevice* device);
		bool check_al_errors(const char* filename, const std::uint_fast32_t line);
		bool check_alc_errors(const char* filename, const std::uint_fast32_t line, ALCdevice* device);
		void play(ALuint* sound);
		ALuint* sound_load_ogg(const char* path);

		template<typename alFunction, typename... Params>
		auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
			->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
		{
			auto ret = function(std::forward<Params>(params)...);
			check_al_errors(filename, line);
			return ret;
		}

		template<typename alFunction, typename... Params>
		auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
			->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
		{
			function(std::forward<Params>(params)...);
			return check_al_errors(filename, line);
		}

		template<typename alcFunction, typename... Params>
		auto alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, ALCdevice* device, Params... params)
			->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
		{
			function(std::forward<Params>(params)...);
			return check_alc_errors(filename, line, device);
		}

		template<typename alcFunction, typename ReturnType, typename... Params>
		auto alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, ReturnType& returnValue, ALCdevice* device, Params... params)
			->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
		{
			returnValue = function(std::forward<Params>(params)...);
			return check_alc_errors(filename, line, device);
		}

	};
}