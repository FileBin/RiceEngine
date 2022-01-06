#include "pch.h"
#include <GameEngine/OggStream.h>

char pcm[BUFFER_SIZE];

namespace Game {

    void OggStream::open(std::string path)
    {
        int result;

        result = fopen_s(&oggFile, path.c_str(), "rb");
        if (result != 0) {
            throw std::wstring(L"Could not open Ogg file");
        }

        if ((result = ov_open(oggFile, &oggStream, NULL, 0)) < 0)
        {
            fclose(oggFile);
            throw std::wstring(L"Could not open Ogg stream: ") + errorString(result);
        }

        vorbisInfo = ov_info(&oggStream, -1);

        if (vorbisInfo->channels == 1)
            format = AL_FORMAT_MONO16;
        else {
            format = AL_FORMAT_STEREO16;
            Log::log(Log::WARNING, L"Stereo sounds will not play in 3d");
        }

        alCall(alGenBuffers, 2, buffers);
        alCall(alGenSources, 1, &source);
        alSourcef(source, AL_GAIN, 0);
    }

    void OggStream::applyEffectChain(std::vector<SoundEffect*> *effects) {
        hasEffects = true;

        alGenFilters(1, &filter);
        alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
        alFilterf(filter, AL_BANDPASS_GAINHF, 0);
        alFilterf(filter, AL_BANDPASS_GAIN, 0);
        alSourcei(source, AL_DIRECT_FILTER, filter);

        if (effects->size() > 1) {
            for (size_t i = 1; i < effects->size(); i++) {
                alAuxiliaryEffectSloti(effects->at(i - 1)->slotID, AL_EFFECTSLOT_TARGET_SOFT, effects->at(i)->slotID);
            }
        }
        alSource3i(source, AL_AUXILIARY_SEND_FILTER, effects->at(0)->slotID, 0, NULL);
        //for more info visit https://www.gamedeveloper.com/programming/openal-s-efx and https://nrgcore.com/docs/manual/en-us/effects_extension_guide.pdf
    }

    void OggStream::setVolume(float volume, bool instant) {
        targetVolume = volume;
        if (instant) {
            alSourcef(source, AL_GAIN, volume);
            currentVolume = volume;
        }
    }

    void OggStream::setPosition(Vector3f position) {
        alSource3f(source, AL_VELOCITY, position.x - prevPos.x, position.y - prevPos.y, position.z - prevPos.z);
        prevPos.x = position.x;
        prevPos.y = position.y;
        prevPos.z = position.z;
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    }

    void OggStream::setLooping(bool looping) {
        alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    }

    void OggStream::setPitch(float pitch) {
        alSourcef(source, AL_PITCH, pitch);
    }

    void OggStream::release()
    {
        alSourceStop(source);
        empty();
        alCall(alDeleteSources, 1, &source);
        alCall(alDeleteBuffers, 2, buffers);

        if (hasEffects) {
            alCall(alDeleteFilters, 1, &filter);
        }

        ov_clear(&oggStream);
    }

    bool OggStream::playback()
    {
        if (playing())
            return true;

        if (!stream(buffers[0]))
            return false;

        if (!stream(buffers[1]))
            return false;

        alSourceQueueBuffers(source, 2, buffers);
        alSourcePlay(source);
        
        return true;
    }

    bool OggStream::playing()
    {
        if (source == 0) {
            return false;
        }

        ALenum state;

        alGetSourcei(source, AL_SOURCE_STATE, &state);

        return (state == AL_PLAYING);
    }

    bool OggStream::update()
    {
        int processed;
        bool active = true;

        if (currentVolume > targetVolume + 0.001f) {
            currentVolume -= 0.001f;
            alSourcef(source, AL_GAIN, currentVolume);
        }
        else if (currentVolume < targetVolume - 0.001f) {
            currentVolume += 0.001f;
            alSourcef(source, AL_GAIN, currentVolume);
        }

        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buffer;

            alCall(alSourceUnqueueBuffers, source, 1, &buffer);

            active = stream(buffer);

            alCall(alSourceQueueBuffers, source, 1, &buffer);
        }

        if (closeOnNoVol) {
            return active && currentVolume > 0.002f;
        }
        else {
            return active;
        }
    }

    bool OggStream::stream(ALuint buffer)
    {
        int  size = 0;
        int  section;
        int  result;

        while (size < BUFFER_SIZE)
        {
            result = ov_read(&oggStream, pcm + size, BUFFER_SIZE - size, 0, 2, 1, &section);

            if (result > 0)
                size += result;
            else
                if (result < 0)
                    throw errorString(result);
                else
                    break;
        }

        if (size == 0)
            return false;

        alCall(alBufferData, buffer, format, pcm, size, vorbisInfo->rate);
        
        return true;
    }

    void OggStream::closeOnNoVolume(bool close) {
        closeOnNoVol = close;
    }

    void OggStream::empty()
    {
        int queued;

        alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

        while (queued--)
        {
            ALuint buffer;
            alCall(alSourceUnqueueBuffers, source, 1, &buffer);
        }
    }

    std::wstring OggStream::errorString(int code)
    {
        switch (code)
        {
        case OV_EREAD:
            return std::wstring(L"Read from media");
        case OV_ENOTVORBIS:
            return std::wstring(L"Not Vorbis data");
        case OV_EVERSION:
            return std::wstring(L"Vorbis version mismatch");
        case OV_EBADHEADER:
            return std::wstring(L"Invalid Vorbis header");
        case OV_EFAULT:
            return std::wstring(L"Internal logic fault (bug or heap/stack corruption)");
        default:
            return std::wstring(L"Unknown Ogg error");
        }
    }
}