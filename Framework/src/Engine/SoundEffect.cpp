#include "pch.h"
#include <GameEngine/SoundEffect.h>

namespace Game {
	SoundEffect::SoundEffect(ALenum effect) {
        alCall(alGenEffects, 1, &effectID);
        alCall(alEffecti, effectID, AL_EFFECT_TYPE, effect);
        
        alCall(alGenAuxiliaryEffectSlots, 1, &slotID);
        alAuxiliaryEffectSloti(slotID, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE);
        alCall(alAuxiliaryEffectSloti, slotID, AL_EFFECTSLOT_EFFECT, effectID);
	}

    SoundEffect::~SoundEffect() {
        alCall(alDeleteAuxiliaryEffectSlots, 1, &slotID);
        alCall(alDeleteEffects, 1, &effectID);
    }

    void SoundEffect::setPropertyf(ALenum property, ALfloat value) {
        alEffectf(effectID, property, value);
    }

    void SoundEffect::setPropertyi(ALenum property, ALint value) {
        alEffecti(effectID, property, value);
    }

    void SoundEffect::setPropertyfv(ALenum property, const ALfloat* values) {
        alEffectfv(effectID, property, values);
    }

    void SoundEffect::setPropertyiv(ALenum property, const ALint* values) {
        alEffectiv(effectID, property, values);
    }

    void SoundEffect::setEfxReverbPreset(EFXEAXREVERBPROPERTIES preset) {
        alEffectf(effectID, AL_EAXREVERB_DENSITY, preset.flDensity);
        alEffectf(effectID, AL_EAXREVERB_DIFFUSION, preset.flDiffusion);
        alEffectf(effectID, AL_EAXREVERB_GAIN, preset.flGain);
        alEffectf(effectID, AL_EAXREVERB_GAINHF, preset.flGainHF);
        alEffectf(effectID, AL_EAXREVERB_GAINLF, preset.flGainLF);
        alEffectf(effectID, AL_EAXREVERB_DECAY_TIME, preset.flDecayTime);
        alEffectf(effectID, AL_EAXREVERB_DECAY_HFRATIO, preset.flDecayHFRatio);
        alEffectf(effectID, AL_EAXREVERB_DECAY_LFRATIO, preset.flDecayLFRatio);
        alEffectf(effectID, AL_EAXREVERB_REFLECTIONS_GAIN, preset.flReflectionsGain);
        alEffectf(effectID, AL_EAXREVERB_REFLECTIONS_DELAY, preset.flReflectionsDelay);
        alEffectfv(effectID, AL_EAXREVERB_REFLECTIONS_PAN, preset.flReflectionsPan);
        alEffectf(effectID, AL_EAXREVERB_LATE_REVERB_GAIN, preset.flLateReverbGain);
        alEffectf(effectID, AL_EAXREVERB_LATE_REVERB_DELAY, preset.flLateReverbDelay);
        alEffectfv(effectID, AL_EAXREVERB_LATE_REVERB_PAN, preset.flLateReverbPan);
        alEffectf(effectID, AL_EAXREVERB_ECHO_TIME, preset.flEchoTime);
        alEffectf(effectID, AL_EAXREVERB_ECHO_DEPTH, preset.flEchoDepth);
        alEffectf(effectID, AL_EAXREVERB_MODULATION_TIME, preset.flModulationTime);
        alEffectf(effectID, AL_EAXREVERB_MODULATION_DEPTH, preset.flModulationDepth);
        alEffectf(effectID, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, preset.flAirAbsorptionGainHF);
        alEffectf(effectID, AL_EAXREVERB_HFREFERENCE, preset.flHFReference);
        alEffectf(effectID, AL_EAXREVERB_LFREFERENCE, preset.flLFReference);
        alEffectf(effectID, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, preset.flRoomRolloffFactor);
        alEffecti(effectID, AL_EAXREVERB_DECAY_HFLIMIT, preset.iDecayHFLimit);
    }
}