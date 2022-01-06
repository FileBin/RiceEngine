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
}