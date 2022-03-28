#include "pch.h"
#include <GameEngine/SoundStream.h>
#include <GameEngine\Log.h>

namespace Game {

    void SoundStream::LoadOgg(std::string path)
    {
        int result;

        result = fopen_s(&oggFile, path.c_str(), "rb");
        if (result != 0) {
            throw std::wstring(L"Could not open Ogg file");
        }

        if ((result = ov_open_callbacks(oggFile, &oggStream, NULL, 0, OV_CALLBACKS_NOCLOSE)) < 0)
        {
            fclose(oggFile);
            throw std::wstring(L"Could not open Ogg stream: ") + errorString(result);
        }

        vorbisInfo = ov_info(&oggStream, -1);

        int format;

        if (vorbisInfo->channels == 1)
            format = AL_FORMAT_MONO16;
        else {
            format = AL_FORMAT_STEREO16;
            Log::log(Log::WARNING, L"Stereo sounds will not play in 3d");
        }

        buffer = new AL::SoundBuffer();
        buffer->SetFormat(format);

        /*alCall(alGenBuffers, 2, buffers);
        alCall(alGenSources, 1, &source);
        alCall(alSourcef, source, AL_GAIN, 0);*/  
    }

    void SoundStream::LoadRaw(FrequencyFunc f, dbl beginning, dbl end)
    {
        raw = true;
        rawFunc = f;
        sampleRate = 44100;
        targetPos = end * sampleRate;
        currentPos = beginning * sampleRate;
        step = 1.0 / sampleRate;

        buffer = new AL::SoundBuffer();
        //alCall(alGenBuffers, 2, buffers);
        //alCall(alGenSources, 1, &source);
    }

    //void SoundStream::applyEffectChain(std::vector<SoundEffect*> effects) {
    //    THROW_EXCEPTION("This Function is not implemented!");
    //    hasEffects = true;

    //    alCall(alGenFilters, 1, &filter);
    //    alCall(alFilteri, filter, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
    //    alCall(alFilterf, filter, AL_BANDPASS_GAINHF, 0);
    //    alCall(alFilterf, filter, AL_BANDPASS_GAIN, 0);
    //    alCall(alSourcei, source, AL_DIRECT_FILTER, filter);
    //    alCall(alSourcei, source, AL_DIRECT_FILTER, filter);
    //    alCall(alSourcei, source, AL_ROOM_ROLLOFF_FACTOR, 1);

    //    if (effects.size() > 1) {
    //        for (size_t i = 1; i < effects.size(); i++) {
    //            alCall(alAuxiliaryEffectSloti, effects.at(i - 1)->slotID, AL_EFFECTSLOT_TARGET_SOFT, effects.at(i)->slotID);
    //            //alAuxiliaryEffectSloti(effects.at(i - 1)->slotID, AL_EFFECTSLOT_TARGET_SOFT, effects.at(i)->slotID);
    //        }
    //    }
    //    alCall(alSource3i, source, AL_AUXILIARY_SEND_FILTER, effects.at(0)->slotID, 0, NULL);
    //    //for more info visit https://www.gamedeveloper.com/programming/openal-s-efx and https://nrgcore.com/docs/manual/en-us/effects_extension_guide.pdf*/
    //}

    void SoundStream::setVolume(float volume, bool instant) {
        targetVolume = volume;
        if (instant) {
            buffer->SetVolume(volume);

            currentVolume = volume;
        }
    }

    void SoundStream::setPosition(Vector3f position) {
        buffer->SetPosition(position);
        if (canCalculateVelocity) {
            buffer->SetVelocity(position - prevPos);
        }
        else {
            canCalculateVelocity = true;
        }
        prevPos = position;
    }

    void SoundStream::setLooping(bool looping) {
        buffer->SetLooping(looping);
    }

    void SoundStream::setPitch(float pitch) {
        buffer->SetPitch(pitch);
    }

    void SoundStream::release()
    {
        buffer.Release();
        /*alCall(alSourceStop, source);
        Clear();
        alCall(alDeleteSources, 1, &source);
        alCall(alDeleteBuffers, 2, buffers);

        if (hasEffects) {
            alCall(alDeleteFilters, 1, &filter);
        }*/

        ov_clear(&oggStream);
    }

    bool SoundStream::Play()
    {
        if (buffer->IsPlaying())
            return true;

        if (!stream(0))
            return false;

        //if (!stream(1))
        //    return false;

        /*alCall(alSourceQueueBuffers, source, 2, buffers);
        alCall(alSourcePlay, source);*/

        buffer->Play();
        
        return true;
    }

    bool SoundStream::update()
    {
        int processed;
        bool active = true;

 /*       if (currentVolume > targetVolume + 0.001f) {
            currentVolume -= 0.001f;
            alCall(alSourcef, source, AL_GAIN, currentVolume);
        }
        else if (currentVolume < targetVolume - 0.001f) {
            currentVolume += 0.001f;
            alCall(alSourcef, source, AL_GAIN, currentVolume);
        }

        alCall(alGetSourcei, source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buffer;

            alCall(alSourceUnqueueBuffers, source, 1, &buffer);

            active = stream(buffer);

            alCall(alSourceQueueBuffers, source, 1, &buffer);
        }*/

        if (closeOnNoVol) {
            return active && currentVolume > 0.002f;
        }
        else {
            return active;
        }
    }

    bool SoundStream::stream(int channel, FrequencyFunc func) {
        constexpr size_t bsize = 0x1000;
        data_t buf(bsize);
        if (currentPos < targetPos) {
            for (long i = 0; i < bsize; i++) {
                buf[i] = func(currentPos * step);
                currentPos++;
            }
            buffer->EmplaceData(buf, 44100, channel);
            //alCall(alBufferData, buffer, AL_FORMAT_MONO16, pcm, BUFFER_SIZE, sampleRate);
            return true;
        } else {
            return false;
        }
    }

    bool SoundStream::stream(int channel) {
        num size = 0;
        int  section;
        num  result;

        size_t data_len = ov_pcm_total(&oggStream, -1) * vorbisInfo->channels * 2;

        data_t buf(data_len);
        while (size < data_len) {
            result = ov_read(&oggStream, (char*) buf.data() + size, 0x1000, 0, 2, 1, &section);

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

        buffer->EmplaceData(buf, vorbisInfo->rate, 0);
        buffer->EmplaceData(buf, vorbisInfo->rate, 1);
        //alCall(alBufferData, buffer, format, pcm, size, vorbisInfo->rate);

        return true;
    }

    void SoundStream::closeOnNoVolume(bool close) {
        closeOnNoVol = close;
    }

    void SoundStream::Clear()
    {
        //int queued;

        //alCall(alGetSourcei, source, AL_BUFFERS_QUEUED, &queued);

        //while (queued--)
        //{
        //    ALuint buffer;
        //    alCall(alSourceUnqueueBuffers, source, 1, &buffer);
        //}
    }

    std::wstring SoundStream::errorString(int code)
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