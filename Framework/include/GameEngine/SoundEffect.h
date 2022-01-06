#pragma once

#include "AlDevice.h"

#include <al/al.h>
#include <al/alc.h>
#define AL_ALEXT_PROTOTYPES
#include <al/alext.h>
#include <al/efx.h>

namespace Game {
    class SoundEffect : private AlDevice
    {
    public:
        SoundEffect(ALenum effect);
        ~SoundEffect();
        void setPropertyf(ALenum property, ALfloat value);
        void setPropertyi(ALenum property, ALint value);
        void setPropertyfv(ALenum property, const ALfloat* values);
        void setPropertyiv(ALenum property, const ALint* values);

        ALuint effectID;
        ALuint slotID;
    };
}
