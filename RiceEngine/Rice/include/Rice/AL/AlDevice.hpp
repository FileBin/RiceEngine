#pragma once

#include <string>
#include <iostream>

#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <GameEngine/Log.h>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace Game {
    class AlDevice
    {
    public:
        bool check_al_errors(const std::wstring& filename, const std::uint_fast32_t line)
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

        bool check_alc_errors(const std::wstring& filename, const std::uint_fast32_t line, ALCdevice* device)
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

        bool check_al_errors(const char* filename, const std::uint_fast32_t line)
        {
            return check_al_errors(Util::Utf8ToWstring(std::string(filename)), line);
        }

        bool check_alc_errors(const char* filename, const std::uint_fast32_t line, ALCdevice* device)
        {
            return check_alc_errors(Util::Utf8ToWstring(std::string(filename)), line, device);
        }

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