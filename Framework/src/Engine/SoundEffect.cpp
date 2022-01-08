#include "pch.h"
#include <GameEngine/SoundEffect.h>

namespace Game {
	SoundEffect::SoundEffect(ALenum effect) {
        alCall(alGenEffects, 1, &effectID);
        alCall(alEffecti, effectID, AL_EFFECT_TYPE, effect);
        
        alCall(alGenAuxiliaryEffectSlots, 1, &slotID);
        alCall(alAuxiliaryEffectSloti, slotID, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE);
        alCall(alAuxiliaryEffectSloti, slotID, AL_EFFECTSLOT_EFFECT, effectID);
	}

    SoundEffect::~SoundEffect() {
        alCall(alDeleteAuxiliaryEffectSlots, 1, &slotID);
        alCall(alDeleteEffects, 1, &effectID);
    }

    void SoundEffect::setPropertyf(ALenum property, ALfloat value) {
        alCall(alEffectf, effectID, property, value);
    }

    void SoundEffect::setPropertyi(ALenum property, ALint value) {
        alCall(alEffecti, effectID, property, value);
    }

    void SoundEffect::setPropertyfv(ALenum property, const ALfloat* values) {
        alCall(alEffectfv, effectID, property, values);
    }

    void SoundEffect::setPropertyiv(ALenum property, const ALint* values) {
        alCall(alEffectiv, effectID, property, values);
    }

    void SoundEffect::setEfxReverbPreset(EFXEAXREVERBPROPERTIES preset) {
        alCall(alEffectf, effectID, AL_EAXREVERB_DENSITY, preset.flDensity);
        alCall(alEffectf, effectID, AL_EAXREVERB_DIFFUSION, preset.flDiffusion);
        alCall(alEffectf, effectID, AL_EAXREVERB_GAIN, preset.flGain);
        alCall(alEffectf, effectID, AL_EAXREVERB_GAINHF, preset.flGainHF);
        alCall(alEffectf, effectID, AL_EAXREVERB_GAINLF, preset.flGainLF);
        alCall(alEffectf, effectID, AL_EAXREVERB_DECAY_TIME, preset.flDecayTime);
        alCall(alEffectf, effectID, AL_EAXREVERB_DECAY_HFRATIO, preset.flDecayHFRatio);
        alCall(alEffectf, effectID, AL_EAXREVERB_DECAY_LFRATIO, preset.flDecayLFRatio);
        alCall(alEffectf, effectID, AL_EAXREVERB_REFLECTIONS_GAIN, preset.flReflectionsGain);
        alCall(alEffectf, effectID, AL_EAXREVERB_REFLECTIONS_DELAY, preset.flReflectionsDelay);
        alCall(alEffectfv, effectID, AL_EAXREVERB_REFLECTIONS_PAN, preset.flReflectionsPan);
        alCall(alEffectf, effectID, AL_EAXREVERB_LATE_REVERB_GAIN, preset.flLateReverbGain);
        alCall(alEffectf, effectID, AL_EAXREVERB_LATE_REVERB_DELAY, preset.flLateReverbDelay);
        alCall(alEffectfv, effectID, AL_EAXREVERB_LATE_REVERB_PAN, preset.flLateReverbPan);
        alCall(alEffectf, effectID, AL_EAXREVERB_ECHO_TIME, preset.flEchoTime);
        alCall(alEffectf, effectID, AL_EAXREVERB_ECHO_DEPTH, preset.flEchoDepth);
        alCall(alEffectf, effectID, AL_EAXREVERB_MODULATION_TIME, preset.flModulationTime);
        alCall(alEffectf, effectID, AL_EAXREVERB_MODULATION_DEPTH, preset.flModulationDepth);
        alCall(alEffectf, effectID, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, preset.flAirAbsorptionGainHF);
        alCall(alEffectf, effectID, AL_EAXREVERB_HFREFERENCE, preset.flHFReference);
        alCall(alEffectf, effectID, AL_EAXREVERB_LFREFERENCE, preset.flLFReference);
        alCall(alEffectf, effectID, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, preset.flRoomRolloffFactor);
        alCall(alEffecti, effectID, AL_EAXREVERB_DECAY_HFLIMIT, preset.iDecayHFLimit);
    }
}