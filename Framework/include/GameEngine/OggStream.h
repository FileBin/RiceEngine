#pragma once

#include <string>
#include <iostream>

#include <al/al.h>
#include <al/alc.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include "AlDevice.h"

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

    protected:

        bool stream(ALuint buffer);
        void empty();
        std::wstring errorString(int code);

    private:

        FILE* oggFile;
        OggVorbis_File  oggStream;
        vorbis_info* vorbisInfo;
        vorbis_comment* vorbisComment;

        ALuint buffers[2];
        ALuint source;
        ALenum format;
    };
}
