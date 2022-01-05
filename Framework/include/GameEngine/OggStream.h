#pragma once

#include <string>
#include <iostream>

#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include "AlDevice.h"

#include <GameEngine/Vectors/Vector3f.h>

#define BUFFER_SIZE (4096 * 4)

namespace Game {
    class OggStream : private AlDevice
    {
    public:

        void open(std::string path);
        void release();
        bool playback();
        bool playing();
        bool update();

        void setVolume(float volume, bool instant);
        void setPosition(Vector3f position);
        void setVelocity(Vector3f velocity);
        void setLooping(bool looping);
        void setPitch(float pitch);
        void closeOnNoVolume(bool close);

    protected:

        bool stream(ALuint buffer);
        void empty();
        std::wstring errorString(int code);
        
    private:

        FILE* oggFile;
        OggVorbis_File  oggStream;
        vorbis_info* vorbisInfo;

        ALuint buffers[2];
        ALuint source;
        ALenum format;
    };
}
