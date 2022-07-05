#pragma once

#include "../stdafx.hpp"

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>


#include "SoundEffect.hpp"


namespace Game {

    typedef char (*FrequencyFunc)(double);

    class SoundStream
    {
    public:

        void loadOgg(std::string path);
        void loadRaw(FrequencyFunc f, dbl beginning, dbl end);
        void release();
        bool Play();
        bool IsPlaying() { return buffer->IsPlaying(); }
        bool update();

        void setVolume(float volume, bool instant);
        void setPosition(Vector3f position);
        void setLooping(bool looping);
        void setPitch(float pitch);
        void closeOnNoVolume(bool close);

        //void applyEffectChain(std::vector<SoundEffect*> effects);

    protected:

        bool stream(int channel);
        bool stream(int channel, FrequencyFunc func);
        void Clear();
        std::wstring errorString(int code);
        
    private:

        FILE* oggFile;
        OggVorbis_File oggStream;
        vorbis_info* vorbisInfo;

        SmartPtr<AL::SoundBuffer> buffer;

        bool closeOnNoVol = false;
        float targetVolume = 1;
        float currentVolume = 0;
        Vector3f prevPos = {};
        bool canCalculateVelocity = false;

        bool hasEffects;
        ALuint filter;

        bool raw;
        FrequencyFunc rawFunc;
        int sampleRate;
        double step;
        long currentPos;
        long targetPos;
    };
}
