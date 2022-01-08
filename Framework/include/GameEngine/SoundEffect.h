#pragma once

#include "AlDevice.h"

#include <al/al.h>
#include <al/alc.h>
#define AL_ALEXT_PROTOTYPES
#include <al/alext.h>
#include <al/efx.h>
#include <al/efx-presets.h>

namespace Game {

    enum Effect {
        REVERB = AL_EFFECT_REVERB,
        CHORUS = AL_EFFECT_CHORUS,
        DISTORTION = AL_EFFECT_DISTORTION,
        ECHO = AL_EFFECT_ECHO,
        FLANGER = AL_EFFECT_FLANGER,
        FREQUENCY_SHIFTER = AL_EFFECT_FREQUENCY_SHIFTER,
        VOCAL_MORPHER = AL_EFFECT_VOCAL_MORPHER,
        PITCH_SHIFTER = AL_EFFECT_PITCH_SHIFTER,
        RING_MODULATOR = AL_EFFECT_RING_MODULATOR,
        AUTOWAH = AL_EFFECT_AUTOWAH,
        COMPRESSOR = AL_EFFECT_COMPRESSOR,
        EQUALIZER = AL_EFFECT_EQUALIZER,
        EAXREVERB = AL_EFFECT_EAXREVERB
    };

    class SoundEffect : private AlDevice
    {
    public:
        SoundEffect(ALenum effect);
        ~SoundEffect();
        void setPropertyf(ALenum property, ALfloat value);
        void setPropertyi(ALenum property, ALint value);
        void setPropertyfv(ALenum property, const ALfloat* values);
        void setPropertyiv(ALenum property, const ALint* values);
        void setEfxReverbPreset(EFXEAXREVERBPROPERTIES preset);
        void setReverbPreset(EFXEAXREVERBPROPERTIES preset);
        ALuint effectID;
        ALuint slotID;
    };
}
