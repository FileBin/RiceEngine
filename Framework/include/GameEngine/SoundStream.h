#pragma once

#include <al/al.h>
#include <al/alc.h>
#define AL_ALEXT_PROTOTYPES
#include <al/alext.h>
#include <al/efx.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include "AlDevice.h"
#include "SoundEffect.h"

#include <GameEngine/Vectors/Vector3f.h>

#define BUFFER_SIZE (4096 * 4)

namespace Game {

    typedef char (*FrequencyFunc)(double);

    class SoundStream : private AlDevice
    {
    public:

        void playOgg(std::string path);
        void playRaw(FrequencyFunc f, dbl beginning, dbl end);
        void release();
        bool playback();
        bool playing();
        bool update();

        void setVolume(float volume, bool instant);
        void setPosition(Vector3f position);
        void setLooping(bool looping);
        void setPitch(float pitch);
        void closeOnNoVolume(bool close);

        void applyEffectChain(std::vector<SoundEffect*>* effects);

    protected:

        bool stream(ALuint buffer);
        void empty();
        std::wstring errorString(int code);
        
    private:

        FILE* oggFile;
        OggVorbis_File oggStream;
        vorbis_info* vorbisInfo;

        ALuint buffers[2];
        ALuint source;
        ALenum format;

        bool closeOnNoVol = false;
        float targetVolume = 1;
        float currentVolume = 0;
        Vector3f prevPos = {};

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
