#include "Rice/Engine/EngineBase.hpp"
#include "pch.h"
NSP_ENGINE_BEGIN

dbl EngineBase::getFixedDeltaTime() { return lock_core()->fixedDeltaTime; }

dbl EngineBase::getDeltaTime() { return lock_core()->deltaTime; }

dbl EngineBase::getTime() { return lock_core()->time; }

EngineBase::EngineBase(ptr<CoreBase> core) : core(core) {}

EngineBase::~EngineBase() {}

ptr<CoreBase> EngineBase::lock_core() {
    auto core_lock = core.lock();
    if (!core_lock)
        THROW_EXCEPTION("Core is not alive!");
    return core_lock;
}

NSP_ENGINE_END